#include "includes.hpp"

using std::string;

std::map<string,Client*> clients_waiting_for_conection;
std::mutex map_mutex;
sqlite3 *db;

string full_read_string(int fd, int size,int & status)
{
   char * buffor = new char[size+1];
   memset(buffor,0,size+1);
   
   
   status = read(fd,buffor,size);
   string s(buffor);
   delete[] buffor;

   return s;
}
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void record_message_in_database(string client_id,string message, bool seen =0){

   string sql = "insert into messeges (client_id, message,seen) values('"+client_id+"','"+message+"','" + (seen?"1":"0") +"');";

   char* errmsg;
   int rc = sqlite3_exec(db,sql.c_str(),callback,0,&errmsg);
   if(rc != SQLITE_OK){
      fprintf(stderr,"sql error: %s\n",errmsg);
      sqlite3_free(errmsg);
   }

}









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
         printf("second time client\n");
         Client* client = clients_waiting_for_conection[client_id];
         client->write_file_descryptor = client_file_descryptor;
         clients_waiting_for_conection.erase(client_id);

         write(client_file_descryptor,"CONNECTED",9);
         

         std::unique_lock conected_lock(conected_mutex);
         if (connected_clients.find(client_id)!=connected_clients.cend())
            connected_clients[client_id]= client;
         else
            connected_clients.insert(std::pair(client_id,client));
         
         client->lunch_threads();


         string statement = "insert into users (client_id) values('" + client_id + "');";
         sqlite3_stmt * stmt;
         sqlite3_prepare(db,statement.c_str(),-1,&stmt,NULL);
         sqlite3_step(stmt);
         sqlite3_finalize(stmt);
         

         

      }
      else{
         printf("first time client\n");
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

   

   int cannot_open_database = sqlite3_open_v2("serwer.db",&db,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,NULL);
   if (cannot_open_database){
      std::cerr<<"ERROR CANNOT OPEN DATABASE\n"<< sqlite3_errmsg(db)<<"\n";


   }

   const char * sql = "create table users( client_id char(10) primary key not null);";
   const char *sql2 = "create table messeges( client_id char(10) not null, message char(1024) not null ,seen char(1) not null);";
   char *errmsg;
   int rc = sqlite3_exec(db,sql,callback,0,&errmsg);
   if(rc != SQLITE_OK){
      fprintf(stderr,"sql error: %s\n",errmsg);
      sqlite3_free(errmsg);
   }else{
      printf("table created succesfully\n");
   }
   rc = sqlite3_exec(db,sql2,callback,0,&errmsg);
   if(rc != SQLITE_OK){
      fprintf(stderr,"sql error: %s\n",errmsg);
      sqlite3_free(errmsg);
   }else{
      printf("table created succesfully\n");
   }

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
//int mss = 1024;
//setsockopt(sfd, IPPROTO_TCP, TCP_MAXSEG, &mss, sizeof(mss));

   bind(sfd, (struct sockaddr*) &saddr, sizeof(saddr));
   listen(sfd, 10);
   while(1) {
      sl = sizeof(caddr);
      std::shared_ptr<sockaddr_in> connection_data =std::make_shared<sockaddr_in>();
      cfd = accept(sfd, (struct sockaddr *)connection_data.get(), &sl);

      std::thread thread(handle_connection,cfd,connection_data);
      thread.detach();

      
      
      
 }



}





