#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/mutex.hpp>

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS
  using boost::asio::local::stream_protocol;
#else
  #error THIS_PLATFORM_IS_NOT_SUPPORTED_LOCAL_SOCKETS
#endif

class progress_bar{
private:
  const long max;
  volatile long counter;
  boost::progress_display bar;
  boost::mutex io_mutex;
public:
  progress_bar(long _max) : max(_max), counter(0), bar(_max)  {};

  long increment() {
    boost::mutex::scoped_lock lock(io_mutex);
    if (max <= counter) {
      return -1;
    }
    counter++;
    ++bar;
    return counter;
  }
};

void run(std::string message, progress_bar *bar, std::string end_point) {
  boost::asio::io_service io_service;
  stream_protocol::socket s(io_service);
  try {
    s.connect(stream_protocol::endpoint(end_point));
    while(bar->increment() != -1) {
      boost::asio::write(s, boost::asio::buffer(message));
      char response[1024];
      boost::asio::read(s,
        boost::asio::buffer(response, 1024),
        boost::asio::transfer_at_least(4));
    }

    s.close();
  } catch(std::exception const&  e) {
    std::cerr << e.what() << std::endl;
    return;
  }
}

int main(int argc, char** argv)  {
  namespace po = boost::program_options;
  int num_of_requests = 1000000;
  int num_of_currencies = 4;
  std::string message = "test";
  std::string sock;

  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "Display usage information (this message)")
    ("requests,n", po::value<int>(&num_of_requests), "Number of requests to perform")
    ("concurrency,c", po::value<int>(&num_of_currencies), "Number of multiple requests to make")
    ("sock,s", po::value<std::string>(&sock), "Path to unix domain socket")
    ("message,m", po::value<std::string>(&message), "Send message to socket");

    po::variables_map vm;
    try {
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);
    } catch(std::exception& e) {
      std::cerr << e.what() << std::endl;
      std::cout << desc << std::endl;
      return 1;
    }
    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 1;
    }

  progress_bar bar(num_of_requests);
  boost::timer t;

  boost::thread_group thread_group;
  for (int i = 0; i < num_of_currencies; ++i) {
    thread_group.create_thread(boost::bind(&run, message, &bar, sock));
  }
  thread_group.join_all();

  double elapsed_time = t.elapsed();
  double qps = static_cast<double>(num_of_requests) / elapsed_time;
  std::cout << "Concurrency Level:　" << num_of_currencies << std::endl;
  std::cout << "Num of requests: " << num_of_requests << std::endl;
  std::cout << "Time taken for tests: " << elapsed_time << " sec" << std::endl;
  std::cout << "Requests per second: " << qps  << " req/sec" << std::endl;

  return 0;
}

