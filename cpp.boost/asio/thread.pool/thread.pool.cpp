#include "zmq.hpp"
#include "file_util.hpp"

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/format.hpp>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace boost;
using namespace zmq;

const long CHUNK_SIZE = 25000;
const int  PIPELINE   = 10;

struct work_context
{
  zmq::context_t* ctx;
  string path;
  string url;
  int64_t     chunk_count;
};

void client_thread0(work_context& wc)
{
  ofstream out(wc.path, ios_base::binary);

  socket_t dealer(*wc.ctx, ZMQ_DEALER);
  dealer.connect(wc.url);

  size_t total  = 0;  // total bytes received
  size_t chunks = 0;  // total chunks received

  while (true) 
  {
    dealer.send(buffer("fetch"s), send_flags::sndmore);
    dealer.send(buffer(to_string(total)), send_flags::sndmore);
    dealer.send(buffer(to_string(CHUNK_SIZE)));

    message_t chunk;
    if (auto size = dealer.recv(chunk))
    {
      chunks++;
      out.write(static_cast<char const *>(chunk.data()), *size);
      total += *size;
      if (*size < CHUNK_SIZE)
        break;
    }
    else
    {
      cout << "read failed" << endl;
      break;
    }
  }

  cout << str(format("%d chunks received, %zd bytes\n") % chunks % total);
}

void client_thread1(work_context& wc)
{
  socket_t dealer(*wc.ctx, ZMQ_DEALER);
  dealer.connect(wc.url.c_str());

  cout << "c: " << wc.url << " connect ok" << endl;

  size_t credit = PIPELINE;

  size_t total  = 0; // total bytes received
  size_t chunks = 0; // total chunks received
  size_t offset = 0; // offset of next chunk request

  ofstream out(wc.path.c_str(), ios_base::binary);
  while (true)
  {
    //while (credit && wc.chunk_count > 0)
    while (credit)
    {
      dealer.send(buffer("fetch"s), send_flags::sndmore);
      dealer.send(buffer(to_string(offset)), send_flags::sndmore);
      dealer.send(buffer(to_string(CHUNK_SIZE)));
      offset += CHUNK_SIZE;
      credit--;
      //wc.chunk_count--;
    }

    message_t chunk;
    if (auto size = dealer.recv(chunk))
    {
      chunks++; credit++;
      out.write(static_cast<char const *>(chunk.data()), *size);
      total += *size;
      if (*size < CHUNK_SIZE)
      {
        cout << "client: done..." << endl;
        break;
      }
    }
    else
    {
      cout << "read failed" << endl;
      break;
    }
  }

  cout << str(format("%d chunks received, %zd bytes\n") % chunks % total);
}

// REMARK
// ROUTER socket이 proxy가 아닌 통신용으로 사용될 경우
// envelop의 구조를 고려해서 코딩한다. 즉, identity 해석
//
void server_thread(work_context& wc)
{
  try
  {
    socket_t router(*(wc.ctx), ZMQ_ROUTER);
    router.bind(wc.url.c_str());

    cout << "s: " << wc.url << " bind ok" << endl;

    ifstream in(wc.path.c_str(), ios_base::binary);

    char buffer[CHUNK_SIZE] = { 0 };
    while (true)
    {
      message_t identity;
      auto s0 = router.recv(identity);
      if (!s0) break;

      message_t message;
      string command;
      if (auto size = router.recv(message))
        command = string((char *)message.data(), *size);

      size_t offset = 0;
      if (auto size = router.recv(message))
        offset = stoi(string((char *)message.data(), *size));

      size_t chunk_sz = 0;
      if (auto size = router.recv(message))
      {
        chunk_sz = stoi(string((char *)message.data(), *size));
        if (chunk_sz == 0)
          break;
      }

      in.seekg(offset, ios_base::beg);
      in.read (buffer, chunk_sz);
      auto actual_read = in.gcount();

      auto to_send = std::min((int)actual_read, (int)chunk_sz);
      router.send(identity, send_flags::sndmore);
      router.send(const_buffer(buffer, to_send), send_flags::none);

      if (actual_read < CHUNK_SIZE)
        break;
    }
  }
  catch(error_t e)
  {
    cout << wc.url << endl;
    cout << e.what() << endl;
  }

  cout << "server: done..." << endl;
}

int main_copy(int argc, char *argv[])
{
  auto src  = "/Users/yielding/Desktop/a.mkv";
  auto dst  = "/Users/yielding/Desktop/copy.mkv";
  auto size = util::file::size(src);
  if (size == 0)
    exit(EXIT_FAILURE);

  context_t context(2);
  int cs = CHUNK_SIZE, 
      cc = (size + cs - 1) / cs;

  work_context server_ctx = { 
    .ctx  = &context, 
    .path = src,
    .url  = "tcp://*:6001",
    .chunk_count = cc
  };

  work_context client_ctx = { 
    .ctx  = &context, 
    .path = dst,
    .url  = "tcp://127.0.0.1:6001",
    .chunk_count = cc
  };

  asio::thread_pool pool(4);
  asio::post(pool, [&] { server_thread (server_ctx); });
  asio::post(pool, [&] { client_thread1(client_ctx); });

  pool.join();
  return 0;
}

int main(int argc, char *argv[])
{
  auto base = "/Users/yielding/Desktop/"s;

  context_t context(4);

  asio::thread_pool server_pool(4);
  asio::thread_pool client_pool(4);

  vector<string> files = { "1.mkv", "2.mkv", "3.mkv" };
  vector<work_context> sctx;

  int connection_count = 3;

  for (int i=0; i<connection_count; i++)
  {
    work_context server_ctx = { 
      .ctx  = &context, 
      .path = base + files[i],
      .url  = "tcp://*:600"s + to_string(i + 1),
      .chunk_count = 0
    };

    sctx.push_back(server_ctx);
  }

  for (auto& ctx: sctx) 
    asio::post(server_pool, [&] { server_thread (ctx); });

  vector<string> files2 = { "a.mkv", "b.mkv", "c.mkv" };
  vector<work_context> cctx;
  for (int i=0; i<connection_count; i++)
  {
    work_context client_ctx = { 
      .ctx  = &context, 
      .path = base + files2[i],
      .url  = "tcp://127.0.0.1:600" + to_string(i + 1),
      .chunk_count = 0
    };

    cctx.push_back(client_ctx);
  }

  for (auto& ctx: cctx)
    asio::post(client_pool, [&] { client_thread1(ctx); });

  client_pool.join();
  server_pool.join();

  exit(EXIT_FAILURE);

  return 0;
}
