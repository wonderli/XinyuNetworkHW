On stdsun system, please run make to compile ftpc.c and ftps.c 
How to run server:
./ftps local-port
For example:
./ftps 5000
How to run client:
./ftpc remote-IP remote-port local-file-to transfer
For example:
./ftpc kappa 5000 1.jpg

After run the ftps and ftpc, the file will be transmitted to the server part, under the ftps program folder, there will have a recv folder contain the file which is sent from client.

