//   1). This implementation of services is meant to provide ease in management
//   of micro-services within the monolith application architecture.
//
//   2) Micro-services are generally threaded code that runs as part of the
//   application. In many cases, they need to talk to each other, which is done
//   by interfaces provided in this file.
//
//   3) Services are started in controlled order:
//      a) allocate the memory
//      b) start the threads
//
//   4) Services are also stopped in a controlled order:
//      a) stop the threads
//      b) wait for the threads to finish
//      c) free up the memory.
//
//   ** It is done in such way so no service can be started and then try to
//      talk to another service that may not exist yet.
//
// ** Example: (services will be started in the order of declaration)
// static NewService service_table[] = { new_service_one, new_service_two, 0 };
// std::vector<IService*> services;
// Parts parts;
//
// ** Create services:
// for (uint n = 0: service_table[n]; n++)
// {
//   IService* s;
//   s = service_table[n](&parts);
//   // ** A service can be NULL if not applicable to the hardware
//   if (s) services.push_back(s);
// }
//
// ** Start services:
// for ( auto& service : services) service->start(&parts);
// while (<execution_condition> + /*signal_handler*/) { // your code }
//
// ** Stop the threads of micro-services:
// for (auto service : services) service->cancel();
//
// ** Join the threads of micro-services:
// for (auto service : services) service->join();
//
// ** Destroy the micro-service to free memory and resources
// for (auto service : services) service->destroy();

#pragma once

// Struct that holds pointers to all created services. When threaded class is
// declared as service with the DECLARE_SERVICE macro it will be provided
// pointer to the Parts through start() function signature so it can be stored
// for accessing other services APIs.
//
// ** In your service_one.h just need to add one macro:
//    SERVICE_CREATE_PROTO(service_one_name);
//
// ** In your service_one.cpp example of micro-service implementation:
//
// class ServiceOne : public IServiceOne
// {
// public:
//    ServiceOne(const Parts*);
//    ~ServiceOne();
//
//    DECLARE_SERVICE(ServiceOne)
//
//    virtual bool function_one() override;
//    virtual bool function_two() const override;
//
// private:
//    const Parts* m_parts;
//    // mutable so function_two() that is marked const can still modify
//    mutable std::mutex m_mutex;
//
//    // -- Locked by m_mutex --
//
//    Set async by main program thread to stop service's processing thread
//    bool m_stop;
//
//    On async task request this CV will be signalled to wake up the thread
//    std::condition_variable m_cv_task;
//
//    // -- End locked by m_mutex --
//
//    // Thread running processor() loop
//    std::thread m_loop_thread;
//    // Loop executing atomic task waiting on cv
//    void processor();
// }
//
// ** Here should follow definitions of the constructor, destructor & methods
//    a) ServiceOne(const Parts* parts) { m_parts = parts; m_stop = false; }
//    b) ~ServiceOne()
//    c) ServiceOne::start(const Parts* parts)
//       { m_parts = parts; m_loop_thread(&ServiceOne::processor, this); }
//    d) ServiceOne::stop()
//       { std::lock_guard<std::mutex> lock(m_mutex);
//         m_stop = true;
//         //wake process so it can stop
//         m_cv_task.notify_all();
//       }
//    e) ServiceOne::join() { m_loop_thread.join(); }
//    f) ServiceOne::destroy() { delete this; }
//    g) ServiceOne::function_one()
//       { std::lock_guard<std::mutex> lock(m_mutex);
//         // Here code your logic to process by the service
//         m_cv_task.notify_one();
//       }
//    h) ServiceOne::function_two() { // Similar logic to fc1 }
//    i) processor()
//       {
//         std::unique_lock<std::mutex> lock(m_mutex);
//         while(!m_stop)
//         {
//           m_cv_task.wait(lock, []() { //code your predicate });
//           if( // logic for task demand) { // code for processing the task };
//         }
//       }
//
//    Lastly create the part itself with the macro:
//    j) CREATE_SERVICE_PART(service_one_name, ServiceOne, service_one)

struct Parts;

class IService {
 public:
  virtual ~IService() {}
  virtual void start(const Parts*) = 0;
  virtual void cancel() = 0;
  virtual void join() = 0;
  // Services are created via a helper in the service module, so the caller
  // never actually calls new. Therefore they should not assume a delete
  // call is appropriate. For example, a service could be a static object
  // and the destroy() function a no-op.
  virtual void destroy() = 0;
  virtual std::string name() = 0;
};

#define DECLARE_SERVICE(service_name)               \
  virtual std::string name() override {             \
    return std::string((const char*)#service_name); \
  }                                                 \
  virtual void start(const Parts*) override;        \
  virtual void cancel() override;                   \
  virtual void join() override;                     \
  virtual void destroy() override;

// Anonymous service with no interface and put into part table
#define CREATE_SERVICE(fcn, cls) \
  IService* fcn(Parts* parts) { return new cls; }

// Create service and insert into part table
#define CREATE_SERVICE_PART(fcn, cls, field) \
  IService* fcn(Parts* parts) {              \
    cls* s = new cls(parts);                 \
    parts->field = s;                        \
    return s;                                \
  }

#define SERVICE_CREATE_PROTO(service_name) IService* service_name(Parts*);

typedef IService* (*NewService)(Parts*);

class IServiceOne : public IService {
 public:
  virtual void function_one() = 0;
  virtual void function_two() const = 0;
};

class IServiceTwo : public IService {
 public:
  virtual void function_one() = 0;
  virtual void function_two() const = 0;
};

struct Parts {
  IServiceOne* service_one;
  IServiceTwo* service_two;

  Parts() {
    service_one = nullptr;
    service_two = nullptr;
  }
};
