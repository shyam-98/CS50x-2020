SELECT
  DISTINCT name
FROM
  people
  INNER JOIN stars ON people.id = stars.person_id
WHERE
  stars.movie_id IN (
    SELECT
      movies.id
    FROM
      movies
      INNER JOIN stars ON movies.id = stars.movie_id
      INNER JOIN people ON stars.person_id = people.id
    WHERE
      people.name = "Kevin Bacon"
      AND people.birth = 1958
  )
  AND name != "Kevin Bacon"