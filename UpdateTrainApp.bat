copy Release\*.exe            %PicesHomeDir%\Exe\
copy Release\*.dll            %PicesHomeDir%\Exe\

copy Release\*.exe            c:\Temp\Pices\Exe\
copy Release\*.dll            c:\Temp\Pices\Exe\


copy  PicesSetup\Release\*    c:\Temp\Pices\SetUp\

mkdir "C:\temp\Pices\MySqlScripts\"
mkdir "C:\temp\Pices\MySqlScripts\Create Scripts\"
copy  "PicesDataBase\Create Scripts\"*.sql   "C:\temp\Pices\MySqlScripts\Create Scripts\"
copy  "PicesDataBase\Create Scripts\"*.bat   "C:\temp\Pices\MySqlScripts\Create Scripts\"

mkdir "C:\temp\Pices\MySqlScripts\StoredProcedures\"
copy  "PicesDataBase\StoredProcedures\"*.sql   "C:\temp\Pices\MySqlScripts\StoredProcedures\"
copy  "PicesDataBase\StoredProcedures\"*.bat   "C:\temp\Pices\MySqlScripts\StoredProcedures\"


mkdir  %PicesHomeDir%\MySqlScripts\
mkdir  %PicesHomeDir%\MySqlScripts\StoredProcedures\
mkdir  %PicesHomeDir%\MySqlScripts\"Create Scripts"\

copy "PicesDataBase\StoredProcedures\"*.sql %PicesHomeDir%\MySqlScripts\StoredProcedures\
copy "PicesDataBase\StoredProcedures\"*.bat %PicesHomeDir%\MySqlScripts\StoredProcedures\

copy  "PicesDataBase\Create Scripts\"*.sql   %PicesHomeDir%\MySqlScripts\"Create Scripts"\
copy  "PicesDataBase\Create Scripts\"*.bat   %PicesHomeDir%\MySqlScripts\"Create Scripts"\

