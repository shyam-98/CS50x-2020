#include "helpers.h"
#include <math.h>

#define MIN(x,y) (((x) < (y)) ? (x) : (y))

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // 'image' is a pointer but we dereference it when we use 'image[i][j]'.
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE pixel = image[i][j]; //copying values
            int n = round((pixel.rgbtBlue + pixel.rgbtGreen + pixel.rgbtRed) / 3.0);
            pixel.rgbtBlue = pixel.rgbtGreen = pixel.rgbtRed = n;
            image[i][j] = pixel;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    int mid = width / 2;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < mid; j++)
        {
            RGBTRIPLE t = image[i][j];
            image[i][j] = image[i][width - 1 - j];
            image[i][width - 1 - j] = t;
        }
    }

    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // stores blurred pixel values temporarily to avoid affecting subsequent calulcations.
    RGBTRIPLE temp[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // (tlx, tly) is top-left corner(inclusive) of the box.
            // (brx, bry) is bottom-right corner(inclusive) of the box.
            int tlx = i - 1;
            int tly = j - 1;
            int brx = i + 1;
            int bry = j + 1;

            // for handling outer edges
            if (tlx < 0) tlx = 0;
            if (tly < 0) tly = 0;
            if (brx > height - 1) brx = height - 1;
            if (bry > width - 1) bry = width - 1;

            int sumBlue = 0, sumGreen = 0, sumRed = 0;
            float count = 0.00; // to ensure floating-point division

            for (int x = tlx; x <= brx; x++)
            {
                for (int y = tly; y <= bry; y++)
                {
                    sumBlue += image[x][y].rgbtBlue;
                    sumGreen += image[x][y].rgbtGreen;
                    sumRed += image[x][y].rgbtRed;
                    count++;
                }
            }

            temp[i][j].rgbtBlue = round(sumBlue / count);
            temp[i][j].rgbtGreen = round(sumGreen / count);
            temp[i][j].rgbtRed = round(sumRed / count);
        }
    }

    // Copying values from 'temp' to 'image'
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtBlue = temp[i][j].rgbtBlue;
            image[i][j].rgbtGreen = temp[i][j].rgbtGreen;
            image[i][j].rgbtRed = temp[i][j].rgbtRed;
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // stores blurred pixel values temporarily to avoid affecting subsequent calulcations.
    RGBTRIPLE temp[height][width];

    int k1[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}; // kernel for detecting edges along x-axis
    int k2[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}}; // kernel for detecting edges along y-axis

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // (tlx, tly) is top-left corner(inclusive) of the box.
            // (brx, bry) is bottom-right corner(inclusive) of the box.
            int tlx = i - 1;
            int tly = j - 1;
            int brx = i + 1;
            int bry = j + 1;

            int blue_channel_box[3][3] = {0};
            int green_channel_box[3][3] = {0};
            int red_channel_box[3][3] = {0};
            int gx_blue = 0, gy_blue = 0;
            int gx_green = 0, gy_green = 0;
            int gx_red = 0, gy_red = 0;
            float g_blue = 0.0;
            float g_green = 0.0;
            float g_red = 0.0;

            for (int x = tlx, x1 = 0; x <= brx; x++, x1++)
            {
                for (int y = tly, y1 = 0; y <= bry; y++, y1++)
                {
                    if (x < 0 || y < 0 || x > (height - 1) || y > (width - 1)) continue;

                    blue_channel_box[x1][y1] = image[x][y].rgbtBlue;
                    green_channel_box[x1][y1] = image[x][y].rgbtGreen;
                    red_channel_box[x1][y1] = image[x][y].rgbtRed;
                }
            }

            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    gx_blue += (blue_channel_box[x][y] * k1[x][y]);
                    gy_blue += (blue_channel_box[x][y] * k2[x][y]);
                    gx_green += (green_channel_box[x][y] * k1[x][y]);
                    gy_green += (green_channel_box[x][y] * k2[x][y]);
                    gx_red += (red_channel_box[x][y] * k1[x][y]);
                    gy_red += (red_channel_box[x][y] * k2[x][y]);
                }
            }

            g_blue = sqrt(pow(gx_blue, 2) + pow(gy_blue, 2));
            g_green = sqrt(pow(gx_green, 2) + pow(gy_green, 2));
            g_red = sqrt(pow(gx_red, 2) + pow(gy_red, 2));

            temp[i][j].rgbtBlue = MIN(round(g_blue), 255);
            temp[i][j].rgbtGreen = MIN(round(g_green), 255);
            temp[i][j].rgbtRed = MIN(round(g_red), 255);
        }
    }

    // Copying values from 'temp' to 'image'
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtBlue = temp[i][j].rgbtBlue;
            image[i][j].rgbtGreen = temp[i][j].rgbtGreen;
            image[i][j].rgbtRed = temp[i][j].rgbtRed;
        }
    }
    return;
}
