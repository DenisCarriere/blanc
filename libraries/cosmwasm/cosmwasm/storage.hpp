#pragma once
#include "memory.hpp"
#include <optional>

namespace cosmwasm {
   namespace internal_use_do_not_use {
      extern "C" {
         WASM_IMPORT("db_read")
         region* db_read(region*);

         WASM_IMPORT("db_write")
         void db_write(region*, region*);

         WASM_IMPORT("db_remove")
         void db_remove(region*);
      }
   }

   struct storage {
      typedef std::vector<char> key_type;
      typedef std::vector<char> value_type;

      std::optional<value_type> get(char* key, uint32_t length) {
         std::unique_ptr<region> key_reg = build_region(key, length);
         region* read = internal_use_do_not_use::db_read(key_reg.get());
         if (!read)
            return std::nullopt;
         std::vector<char> data = consume_region(read);
         return data;
      }

      std::optional<value_type> get(key_type& key) {
         return get(key.data(), key.size());
      }

      std::optional<value_type> get(key_type&& key) {
         return get(key.data(), key.size());
      }

      void set(char* key, uint32_t key_len, char* value, uint32_t value_len) {
         if (!value_len)
            abort();

         std::unique_ptr<region> key_reg = build_region(key, key_len);
         std::unique_ptr<region> value_reg = build_region(value, value_len);
         internal_use_do_not_use::db_write(key_reg.get(), value_reg.get());
      }

      void set(key_type& key, value_type& value) {
         set(key.data(), key.size(), value.data(), value.size());
      }

      void set(key_type&& key, value_type& value) {
         set(key.data(), key.size(), value.data(), value.size());
      }

      void set(key_type& key, value_type&& value) {
         set(key.data(), key.size(), value.data(), value.size());
      }

      void set(key_type&& key, value_type&& value) {
         set(key.data(), key.size(), value.data(), value.size());
      }

      void remove(char* key, uint32_t length) {
         std::unique_ptr<region> key_reg = build_region(key, length);
         internal_use_do_not_use::db_remove(key_reg.get());
      }

      void remove(key_type& key) {
         remove(key.data(), key.size());
      }

      void remove(key_type&& key) {
         remove(key.data(), key.size());
      }
   };

   bytes namespaces_with_key(const std::vector<bytes>& namespaces, bytes key) {
      int size = key.size();
      for (const auto& nm : namespaces) {
         size += nm.size() + 2;
      }

      bytes out(size);
      auto it = out.begin();
      for (const auto& nm : namespaces) {
         *it++ = nm.size() & 0xFF;
         *it++ = (nm.size() >> 8) & 0xFF;
         it = std::copy(nm.begin(), nm.end(), it);
      }
      std::copy(key.begin(), key.end(), it);
      return out;
   }
}
