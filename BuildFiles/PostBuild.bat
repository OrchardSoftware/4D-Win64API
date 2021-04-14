:: Echo the parameters we recieved 
echo %1
echo %2

:: Copy License File
echo F|xcopy /i /y %2BuildFiles\LICENSE %1..\LICENSE

:: Copy Manifest File
echo F|xcopy /i /y %2BuildFiles\manifest.json %1..\manifest.json

:: Copy Constants File
echo F|xcopy /i /y %2BuildFiles\constants.xlf %1..\Resources\constants.xlf
