drop table if exists Instruments;


CREATE TABLE Instruments
(
	InstrumentCode char(3)     NOT NULL,
	Description    varchar(50) NULL,
 
    Primary Key (InstrumentCode  asc)
 );
 
 
 insert into Instruments values ("FLM", "Flow Meter");
 insert into Instruments values ("BAT",  "Batery Meter");
 insert into Instruments values ("P-R", "Pitch and Roll");
 insert into Instruments values ("CTD", "Conductivity Tempereture Depth");
        