DROP TABLE IF EXISTS `custom_deserter`;

CREATE TABLE IF NOT EXISTS `custom_deserter`
(
    `ID`       INT unsigned AUTO_INCREMENT,
    `Guid`     INT unsigned NOT NULL,
    `Time`     INT unsigned NOT NULL,
    `Duration` INT          NOT NULL,
    CONSTRAINT `PK_custom_deserter` PRIMARY KEY(`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

