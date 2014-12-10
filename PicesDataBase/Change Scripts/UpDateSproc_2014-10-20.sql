ALTER TABLE `pices_new`.`instrumentdata` 
    ADD COLUMN `CropLeft`      INT(5) UNSIGNED NULL DEFAULT 100 AFTER `CdomFluorescenceSensor`,
    ADD COLUMN `CropRight`     INT(5) UNSIGNED NULL DEFAULT 3900 AFTER `CropLeft`,
    ADD COLUMN `ActiveColumns` INT(5) UNSIGNED NULL DEFAULT 3801 AFTER `CropRight`;
    
    
ALTER TABLE deployments
    ADD COLUMN `ChamberWidth` FLOAT NULL DEFAULT 0.096 COMMENT 'Width of imaging chamber in Meters.' AFTER `CropRight`;
