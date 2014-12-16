alter table `pices_new`.`instrumentdata` 
    add column `CropLeft`      int(5) UNSIGNED NULL DEFAULT 100  after `CdomFluorescenceSensor`,
    add column `CropRight`     int(5) UNSIGNED NULL DEFAULT 3900 after `CropLeft`,
    add column `ActiveColumns` int(5) UNSIGNED NULL DEFAULT 3801 after `CropRight`;
    
    
alter table  deployments
    ADD COLUMN `ChamberWidth` float null default 0.096 COMMENT 'Width of imaging chamber in Meters.' after `CropRight`;
    
    
alter table classes
    add column Mandatory char(1) not null default 'F' after `Description`;


alter table classes
    add column Summarize char(1) not null default 'F' after Mandatory;
