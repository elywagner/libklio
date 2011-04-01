#ifndef LIBKLIO_STORE_FACTORY_HPP
#define LIBKLIO_STORE_FACTORY_HPP 1

#include <libklio/common.hpp>
#include <libklio/store.hpp>


namespace klio {
  class StoreFactory {
    public:
      typedef std::tr1::shared_ptr<StoreFactory> Ptr;
      StoreFactory () {};
      Store::Ptr createStore(const STORETYPE& type);
      virtual ~StoreFactory() {};

    private:
      StoreFactory (const StoreFactory& original);
      StoreFactory& operator= (const StoreFactory& rhs);
      
  };
  
};


#endif /* LIBKLIO_STORE-FACTORY_HPP */
