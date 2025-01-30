
#include "includes.hpp"


Client::Client(){};


extern std::map<string,Client*> connected_clients;
extern std::shared_mutex conected_mutex;
extern string full_read_string(int fd, int size,int & status);
extern void record_message_in_database(string client_id,string message);


void send_message_to_client(string client_id, string message){

      
      std::shared_lock lock(conected_mutex);

      Client *client = connected_clients[client_id];


      record_message_in_database(client_id,message);

      if (client)
            client->post_message(message);
}


void Client::lunch_threads(){
      reader = std::thread(Client::listen_for_messages,this);
      reader.detach();
      writer = std::thread(Client::send_messages,this);
      
      
      writer.detach();

      printf("client %s has connected\n",client_id.c_str());

}


void Client::listen_for_messages(Client *c){

      string x;
      int status;
      do{
      
      x = full_read_string(c->read_file_descryptor,PACKET_SIZE,status);
      
      printf("josn: %s\n",x.c_str());

      if (status <= 0) break;
      auto ms1 = json::parse(x);

      std::cout<<std::endl<<std::endl<<ms1<<std::endl<<std::endl;

      string recepient = ms1.at("to");
      std::cout<<"recepient: "<<recepient<<std::endl;
      
      send_message_to_client(recepient,x);
      
      
      }while(status > 0);
      printf("%s has closed connection", c->client_id.c_str());
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
      c->has_meseges_to_send.wait(lock);
      
      message = c->messages.front();
      c->messages.pop_front();
      }
      write(c->write_file_descryptor,message.c_str(),message.size());


      }
}









