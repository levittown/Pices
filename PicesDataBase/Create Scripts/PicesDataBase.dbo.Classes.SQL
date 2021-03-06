delimiter ;



drop table if exists Classes;
create table Classes
   (
     ClassId      int unsigned  not null auto_increment,
     ClassName    varchar(64)   not null,
     ParentId     int unsigned  null,
     Description  varchar(255)  null,
     Mandatory    char(1)       not null default 'F'  comment 'Indicates that the Classification report MUST report this class even if no occurrences in the data.',
     Summarize    char(1)       not null default 'F'  comment 'Indicates whether reports should produce a summary for specified parent classes.',

     primary key (ClassId),
     unique key  ClassNameKey   (ClassName  asc),
     key         ParentKey      (ParentId   asc, ClassName asc)
   )
;



