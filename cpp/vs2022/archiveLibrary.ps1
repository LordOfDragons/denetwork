Remove-Item -Path build\archive\DENetwork -Recurse -Force -ErrorAction SilentlyContinue

New-Item -Path build\archive\DENetwork\SDK\Include -ItemType Directory
New-Item -Path build\archive\DENetwork\SDK\Library\x64 -ItemType Directory

Copy-Item -Filter *.h -Path ..\library\src -Recurse -Destination build\archive\DENetwork\SDK\Include
Move-Item build\archive\DENetwork\SDK\Include\src build\archive\DENetwork\SDK\Include\denetwork

Copy-Item x64\Release\DENetwork.lib build\archive\DENetwork\SDK\Library\x64
#Copy-Item x64\Release\DENetwork.pdb build\archive\DENetwork\SDK\Library\x64

Copy-Item ..\..\LICENSE build\archive\DENetwork
Copy-Item ..\..\README.md build\archive\DENetwork

Compress-Archive -Path .\build\archive\DENetwork\* -DestinationPath .\build\archive\DENetworkSDK.zip
