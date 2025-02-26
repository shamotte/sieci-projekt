#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <includes.hpp>
using std::string;

class Client{
   public:
   std::string client_id;
   int read_file_descryptor;
   int write_file_descryptor;

   SSL * ssl_write_descryptor;
   SSL * ssl_read_descryptor;

   std::mutex message_mutex;
   std::list<string> messages;

   std::thread writer,reader;
   std::condition_variable has_meseges_to_send;

   
   Client();

   void lunch_threads();


   static void listen_for_messages(Client *c);

   void post_message(string s);

   static void send_messages(Client *c);

   void post_stored_messages();




   
};





