SELECT
  movies.title
FROM
  movies
  INNER JOIN stars ON movies.id = stars.movie_id
  INNER JOIN people ON stars.person_id = people.id
WHERE
  people.name = "Johnny Depp"
  AND movies.id IN (
    SELECT
      movies.id
    FROM
      movies
      INNER JOIN stars ON movies.id = stars.movie_id
      INNER JOIN people ON stars.person_id = people.id
    WHERE
      people.name = "Helena Bonham Carter"
  )