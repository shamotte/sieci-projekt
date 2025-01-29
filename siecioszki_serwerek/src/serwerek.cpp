#include "includes.hpp"

using std::string;


string full_read_string(int fd, int size,int & status)
{
   char * buffor = new char[size];
   
   
   status = read(fd,buffor,size);
   string s(buffor);

   return s;
}






std::map<string,Client*> clients_waiting_for_conection;
std::mutex map_mutex;


std::map<string,Client*> connected_clients;
std::shared_mutex conected_mutex;

void handle_connection(int client_file_descryptor,std::shared_ptr<sockaddr_in> connection_data){ 
     
      int x;
      string client_id = full_read_string(client_file_descryptor,10,x);
      printf("new connection from %s(%s):%d\n", inet_ntoa((struct in_addr)connection_data->sin_addr),client_id.c_str(),ntohs(connection_data->sin_port));

      
      
      {
            
      std::unique_lock lokc(map_mutex);
      
      if(clients_waiting_for_conection.find(client_id) != clients_waiting_for_conection.cend())
      {
         Client* client = clients_waiting_for_conection[client_id];
         client->write_file_descryptor = client_file_descryptor;
         clients_waiting_for_conection.erase(client_id);

         write(client_file_descryptor,"CONNECTED",9);
         

         std::unique_lock conected_lock(conected_mutex);
         connected_clients.insert(std::pair(client_id,client));
         client->lunch_threads();

         

      }
      else{
         write(client_file_descryptor,"OK",2);
         Client *c = new Client();
         c->client_id = client_id;
         c->read_file_descryptor = client_file_descryptor;
         clients_waiting_for_conection.insert(std::pair<string,Client*>(client_id,c));
      }
      }


}




int main()
{

   sqlite3 *db;

   int cannot_open_database = sqlite3_open_v2("serwer.db",&db,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,NULL);
   if (cannot_open_database){
      std::cerr<<"ERROR CANNOT OPEN DATABASE\n"<< sqlite3_errmsg(db)<<"\n";


   }



#pragma region network_initialization
int sfd, cfd, on =1;
socklen_t sl;
struct sockaddr_in saddr, caddr;
memset(&saddr, 0, sizeof(saddr));
saddr.sin_family = AF_INET;
saddr.sin_addr.s_addr = INADDR_ANY;
saddr.sin_port = htons(1234);
sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on,
sizeof(on));

#pragma endregion

   bind(sfd, (struct sockaddr*) &saddr, sizeof(saddr));
   listen(sfd, 10);
   while(1) {
      sl = sizeof(caddr);
      std::shared_ptr<sockaddr_in> connection_data =std::make_shared<sockaddr_in>();
      cfd = accept(sfd, (struct sockaddr *)connection_data.get(), &sl);

      
      handle_connection(cfd,connection_data);
      
      
 }



}





