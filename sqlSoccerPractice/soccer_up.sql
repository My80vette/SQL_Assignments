DROP TABLE IF EXISTS teams;
DROP TABLE IF EXISTS schedule;
DROP TABLE IF EXISTS players;
DROP TABLE IF EXISTS games;

CREATE TABLE teams (
    team VARCHAR(32) NOT NULL,
    coach VARCHAR(32),
    PRIMARY KEY(team)
);
INSERT INTO teams VALUES
    ('Cardinals','Newcomb'),
    ('Cowboys','Stowers'),
    ('Rampage','Mussleman'),
    ('Ropers','Capriotti'),
    ('Packers','LaFleaur'),
    ('Bears','Nagy');


CREATE TABLE schedule (
    game_id int NOT NULL AUTO_INCREMENT,
    team VARCHAR(32),
    host bool NOT NULL,
    score int,
    PRIMARY KEY (game_id, host)
    -- FOREIGN KEY(team) REFERENCES teams(team) --
    -- ON UPDATE CASCADE --
);
INSERT INTO schedule VALUES
    ('1','Cardinals','1','4'),
    ('1','Bears','0','5'),
    ('2','Cowboys','1','2'),
    ('2','Packers','0','3'),
    ('3','Rampage','1','6'),
    ('3','Ropers','0','7'),
    ('4','Packers','1','4'),
    ('4','Cardinals','0','3'),
    ('5','Ropers','1','4'),
    ('5','Bears','0','6'),
    ('6','Rampage','1','2'),
    ('6','Cowboys','0','8'),
    ('7','Cardinals','1','5'),
    ('7','Ropers','0','4'),
    ('8','Packers','1','1'),
    ('8','Rampage','0','4'),
    ('9','Bears','1','6'),
    ('9','Cowboys','0','7'),
    ('10','Rampage','1','8'),
    ('10','Cardinals','0','2'),
    ('11','Cowboys','1','3'),
    ('11','Ropers','0','4'),
    ('12','Bears','1','1'),
    ('12','Packers','0','5'),
    ('13','Cardinals','1','2'),
    ('13','Cowboys','0','8'),
    ('14','Bears','1','8'),
    ('14','Rampage','0','4'),
    ('15','Ropers','1','5'),
    ('15','Packers','0','6');



CREATE TABLE players (
    first_name VARCHAR(32) NOT NULL,
    last_name VARCHAR(32) NOT NULL,
    team VARCHAR(32),
    PRIMARY KEY(player)
    -- FOREIGN KEY(team) REFERENCES teams(team) --
    -- ON UPDATE CASCADE --
);
INSERT INTO players VALUES
    ('Nick','C.','Cardinals'),
    ('Baylor','R.','Cardinals'),
    ('Gunner','C.','Cardinals'),
    ('Mack','J.','Cardinals'),
    ('Tyler','T.','Cardinals'),
    ('Juice','W.','Cowboys'),
    ('Kanye','W.','Cowboys'),
    ('Aaron','R.','Cowboys'),
    ('Michael','J.','Cowboys'),
    ('Donald','T.','Cowboys'),
    ('Joe','B.','Rampage'),
    ('Mich','J.','Rampage'),
    ('Josh','D.','Rampage'),
    ('Ian','C.','Rampage'),
    ('Pablo','C.','Rampage'),
    ('DeShaun','W.','Ropers'),
    ('Kyler','M.','Ropers'),
    ('Josh','Z.','Ropers'),
    ('Jack','L.','Ropers'),
    ('Jonsey','L.','Ropers'),
    ('Kirby','N.','Packers'),
    ('Chris','P.','Packers'),
    ('Robert','D.','Packers'),
    ('Clark','N.','Packers'),
    ('Josiah','K.','Packers'),
    ('Jordan','L.','Bears'),
    ('Brett','F.','Bears'),
    ('Joe','N.','Bears'),
    ('Throckmorton','S.','Bears'),
    ('Master','C.','Bears');


CREATE TABLE games (
    game_id int NOT NULL AUTO_INCREMENT,
    location VARCHAR(32),
    date DATETIME,
    PRIMARY KEY (game_id)
);
INSERT INTO games VALUES
    ('1', 'Field 1', '2022-10-01 09:00:00'),
    ('2', 'Field 2', '2022-10-01 09:00:00'),
    ('3', 'Field 3', '2022-10-01 09:00:00'),
    ('4', 'Field 1', '2022-10-08 09:00:00'),
    ('5', 'Field 2', '2022-10-08 09:00:00'),
    ('6', 'Field 3', '2022-10-08 09:00:00'),
    ('7', 'Field 1', '2022-10-15 09:00:00'),
    ('8', 'Field 2', '2022-10-15 09:00:00'),
    ('9', 'Field 3', '2022-10-15 09:00:00'),
    ('10', 'Field 1', '2022-10-22 09:00:00'),
    ('11', 'Field 2', '2022-10-22 09:00:00'),
    ('12', 'Field 3', '2022-10-22 09:00:00'),
    ('13', 'Field 1', '2022-10-29 09:00:00'),
    ('14', 'Field 2', '2022-10-29 09:00:00'),
    ('15', 'Field 3', '2022-10-29 09:00:00');
