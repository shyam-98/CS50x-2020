import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    portfolioValue = 0
    holdings = db.execute("SELECT symbol, shares FROM holdings WHERE user_id = ? ORDER BY symbol", session["user_id"])
    for holding in holdings:
        stockData = lookup(holding["symbol"])
        holding["name"] = stockData["name"]
        holding["price"] = stockData["price"]
        holding["total"] = holding["price"] * holding["shares"]
        portfolioValue += holding["total"]

        holding["price"] = usd (holding["price"])
        holding["total"] = usd(holding["total"])

    cashBalance = db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])[0]["cash"]
    portfolioValue += cashBalance

    cashBalance = usd(cashBalance)
    portfolioValue = usd(portfolioValue)

    return render_template("index.html", holdings = holdings, cashBalance = cashBalance, portfolioValue = portfolioValue)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure symbol is submitted
        symbol = request.form.get("symbol").upper()
        if not symbol:
            return apology("missing symbol", 403)

        # Ensure symbol is valid
        stockData = lookup(symbol)
        if stockData is None:
            return apology("invalid symbol")

        # Ensure shares are provided
        try:
            shares = int(request.form.get("shares"))
        except ValueError:
            return apology("missing shares", 403)

        # Ensure the user can afford to purchase the shares
        cash = float(db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])[0]["cash"])
        total = shares * stockData["price"]
        if cash < total:
            return apology("can't afford")

        # Update transactions table
        db.execute("INSERT INTO transactions (user_id, symbol, shares, price, type) VALUES (?, ?, ?, ?, ?)", session["user_id"], symbol, shares, stockData["price"], "BUY")

        # Update cash balance
        db.execute("UPDATE users SET cash = ? WHERE id = ?", (cash - total), session["user_id"])

        # Update holdings
        current_holdings = db.execute("SELECT * FROM holdings WHERE user_id = ? AND symbol = ?", session["user_id"], symbol)
        if len(current_holdings) == 0:
            db.execute("INSERT INTO holdings (user_id, symbol, shares) VALUES (?, ?, ?)", session["user_id"], symbol, shares)
        else:
            db.execute("UPDATE holdings SET shares = ? WHERE user_id = ? AND symbol = ?", current_holdings[0]["shares"] + shares, session["user_id"], symbol)

        flash("Purchase of {} shares of {} complete!".format(shares, stockData["name"]))
        return redirect("/")

    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    history = db.execute("SELECT symbol, shares, price, type, Timestamp FROM transactions WHERE user_id = ?", session["user_id"])
    for row in history:
        row["price"] = usd(row["price"])

    return render_template("history.html", history = history)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        flash("Welcome, {}!".format(rows[0]["username"]))
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Quote stock price"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensuer username was provided
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("missing symbol", 403)

        # Ensure the provided symbol is valid
        stockData = lookup(symbol)
        if stockData is None:
            return apology("invalid symbol")

        stockData["price"] = usd(stockData["price"])

        return render_template("quoted.html", stockData=stockData)

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    if request.method == "POST":

        # Ensure username was submitted
        username = request.form.get("username")
        if not username:
            return apology("must provide username", 403)

        # Query database to ensure no duplicate usernames are allowed
        rows = db.execute("SELECT * FROM users WHERE username = ?", username)
        if len(rows) != 0:
            return apology("username already exists")

        # Ensure password was submitted
        password = request.form.get("password")
        if not password:
            return apology("must provide password", 403)

        # Ensure password and confirmation password match
        if password != request.form.get("confirmation"):
            return apology("Your password and confirmation password do not match.")

        # Insert new users credentials in the database
        db.execute("INSERT INTO users (username, hash) VALUES (?, ?)", username, generate_password_hash(password))

        return redirect("/login")

    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    if request.method == "POST":

        # Ensure symbol is submitted
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("missing symbol", 403)

        # Ensure shares is a positive integer
        try:
            sellQuantity = int(request.form.get("shares"))
        except ValueError:
            return apology("missing shares", 403)
        if sellQuantity < 1:
            return apology("shares must be greater than or equal to 1")

        sharesInAccount = db.execute("SELECT shares FROM holdings WHERE user_id = ? AND symbol = ?", session["user_id"], symbol)[0]["shares"]

        # Ensure user holds sufficient shares
        if sellQuantity > sharesInAccount:
            return apology("Insufficient shares in account")

        stockData = lookup(symbol)

        # Update transaction logs
        db.execute("INSERT INTO transactions (user_id, symbol, shares, price, type) VALUES (?, ?, ?, ?, ?)", session["user_id"], symbol, -sellQuantity, stockData["price"], "SELL")

        # Update users holdings
        updatedSharesInAccount = sharesInAccount - sellQuantity
        if updatedSharesInAccount == 0:
            db.execute("DELETE FROM holdings WHERE user_id = ? AND symbol = ?", session["user_id"], symbol)
        else:
            db.execute("UPDATE holdings SET shares = ? WHERE user_id = ? AND symbol = ?", updatedSharesInAccount, session["user_id"], symbol)

        # Update users Cash Balance
        oldBalance = db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])[0]["cash"]
        newBalance = oldBalance + (sellQuantity * stockData["price"])
        db.execute("UPDATE users SET cash = ? WHERE id = ?", newBalance, session["user_id"])

        flash("Sold!")
        return redirect("/")

    else:
        symbols = db.execute("SELECT symbol FROM holdings WHERE user_id = ? ORDER BY symbol", session["user_id"])
        return render_template("sell.html", symbols = symbols)

@app.route("/change_password", methods=["GET", "POST"])
@login_required
def change_password():
    """Change password"""

    if request.method == "POST":

        currentPassword = request.form.get("currentPassword")
        if not currentPassword:
            return apology("must provide the password", 403)

        newPassword = request.form.get("newPassword")
        if not newPassword:
            return apology("must provide a new password", 403)

        newPasswordConfirmation = request.form.get("newPasswordConfirmation")
        if newPassword != newPasswordConfirmation:
            return apology("new password and confirmation password do not match.", 403)

        currentHash = db.execute("SELECT hash FROM users WHERE id = ?", session["user_id"])[0]["hash"]
        if not check_password_hash(currentHash, currentPassword):
            return apology("Invalid Password", 403)

        if check_password_hash(currentHash, newPassword):
            return apology("New password cannot be the same as old password.", 403)

        db.execute("Update users SET hash = ? WHERE id = ?", generate_password_hash(newPassword), session["user_id"])

        flash("Password Changed Successfully!")
        return redirect("/change_password")
    else:
        return render_template("change_password.html")

def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
