CREATE DATABASE IF NOT EXISTS `what`;

USE `what`;

CREATE TABLE IF NOT EXISTS `Account` (
	`ID` integer NOT NULL auto_increment,
	`Name` varchar(30) NOT NULL,
	`Password` varchar(30) NOT NULL,
	PRIMARY KEY (`ID`)
) ENGINE = InnoDB;

CREATE TABLE IF NOT EXISTS `Progress` (
	`ID` integer NOT NULL auto_increment,
	`AccountID` integer NOT NULL,
	`Name` varchar(30) NOT NULL,
	`Score` integer DEFAULT NULL,
	PRIMARY KEY (`ID`),
	FOREIGN KEY (`AccountID`) REFERENCES `Account`(`ID`)
) ENGINE = InnoDB;
