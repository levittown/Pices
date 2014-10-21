ALTER TABLE deployments
    ADD COLUMN `ChamberWidth` FLOAT NULL DEFAULT 96.0 COMMENT 'Width of imaging chamber in Mili-Meters.' AFTER `CropRight`;
