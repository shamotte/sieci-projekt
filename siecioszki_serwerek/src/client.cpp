
#include "includes.hpp"


Client::Client(){};


extern std::map<string,Client*> connected_clients;
extern std::shared_mutex conected_mutex;
extern string full_read_string(int fd, int size,int & status);
extern void record_message_in_database(string client_id,string message,bool seen =1);
extern sqlite3 *db;

static int select_callback(void* data, int argc,char**argv,char ** azColName)
{
      printf("posting stored message to client\n");
      Client *c = (Client*)data;
      
      string message(argv[0]);
      message[9]= argv[1][0];
      c->post_message(message.c_str());
      return 0;
      
}
static int update_callback(void* data,int argc,char ** argv,char ** colName){
      return 0;
}

void Client::post_stored_messages(){

      string select_sql = "select message,seen from messeges where client_id like '"+client_id+"';";
      string update_sql = "update messeges set seen = '1' where client_id like '"+client_id+"';";


      char* errmsg;
      int rc = sqlite3_exec(db,select_sql.c_str(),select_callback,(void*)this,&errmsg);
     
      rc = sqlite3_exec(db,update_sql.c_str(),update_callback,(void*)this,&errmsg);
      if(rc != SQLITE_OK){
            fprintf(stderr,"sql error: %s\n",errmsg);
            sqlite3_free(errmsg);
      }else{
            printf("record updated succesfully\n");
      }
      

}


void send_message_to_client(string client_id, string message){

      
      std::shared_lock lock(conected_mutex);

      Client *client = connected_clients[client_id];


      record_message_in_database(client_id,message, client?true:false);

      if (client)
            client->post_message(message);
}


void Client::lunch_threads(){
      reader = std::thread(Client::listen_for_messages,this);
      reader.detach();
      writer = std::thread(Client::send_messages,this);
      
      printf("client %s has connected\n",client_id.c_str());
      post_stored_messages();

      

}


void Client::listen_for_messages(Client *c){

      string x;
      int status;
      do{
      
      x = full_read_string(c->read_file_descryptor,PACKET_SIZE,status);
      
      printf("josn: %s\n",x.c_str());

      if (status <= 0) break;
      auto message_json = json::parse(x);

      std::cout<<std::endl<<std::endl<<message_json<<std::endl<<std::endl;

      string recepient = message_json.at("to");
      std::cout<<"recepient: "<<recepient<<std::endl;
      string sender = message_json.at("from");
      
      send_message_to_client(recepient,x);
      record_message_in_database(sender,x);
      
      
      }while(status > 0);
      printf("%s has closed connection", c->client_id.c_str());
      connected_clients[c->client_id] = nullptr;
        
}

void Client::post_message(string s){
      {
      std::unique_lock lock(message_mutex);
      messages.push_back(s);
      
      }
      has_meseges_to_send.notify_one();
}

void Client::send_messages(Client *c){

      while(true)
      {
      
      string message;
      {
      
      std::unique_lock lock(c->message_mutex);
      if(c->messages.size() < 1)
            c->has_meseges_to_send.wait(lock);
      
      message = c->messages.front();
      c->messages.pop_front();
      }
      write(c->write_file_descryptor,message.c_str(),message.size());


      }
}









