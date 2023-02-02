SELECT player, teams.team, coach 
FROM players
JOIN teams on teams.team = players.team;

SELECT player, date, host
FROM players
JOIN schedule ON schedule.team = players.team
JOIN games ON schedule.game_id = games.game_id
WHERE player = "Aaron R.";

SELECT player, date, t1.host, t2.team, coach
FROM
(SELECT player, date, host, schedule.game_id, schedule.team
FROM players
JOIN schedule ON schedule.team = players.team
JOIN games ON schedule.game_id = games.game_id
WHERE player = "Aaron R.") as t1
JOIN
(SELECT schedule.team, coach, schedule.game_id
FROM schedule
JOIN teams ON schedule.team = teams.team) as t2
on t2.game_id = t1.game_id
WHERE t1.team != t2.team;

