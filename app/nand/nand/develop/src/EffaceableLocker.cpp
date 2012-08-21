#include "EffaceableLocker.h"

#include <boost/crc.hpp>
#include <iostream>

using namespace std;
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace {

    enum LockerValue {
        Dkey = 0x446B6579,
        EMF  = 0x454D4621,
        BAG1 = 0x42414731,
        DONE = 0x444f4e45
    };

    struct Locker
    {
        string     magic;
        uint16_t   length;   // unsigned little endian
        ByteBuffer tag;      // union tas: as_integer, as_string
    };
   
    // String: ByteBuffer or std::string
    template <typename String> 
    string xor_strings(String const& s, String const& key)
    {
        string res;

        for (auto i=0; i<s.size(); ++i)
        {
            uint8_t a = s[i];
            uint8_t b = key[i % key.size()];
            res += char(a ^ b);
        }

        return res;
    }

    auto crc32(ByteBuffer b) -> uint32_t
    {
        boost::crc_32_type result;
        result.process_bytes(b, b.size());

        return result.checksum();
    }
} 

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
EffaceableLockers::EffaceableLockers(ByteBuffer data)
{
}

auto EffaceableLockers::get_emf(string k89b) -> ByteBuffer
{
    return utility::hex::ByteBuffer();
}

auto EffaceableLockers::get_dkey(string k835) -> ByteBuffer
{
    return utility::hex::ByteBuffer();
}

auto EffaceableLockers::get_locker(string tag) -> ByteBuffer
{
    return utility::hex::ByteBuffer();
}

auto EffaceableLockers::to_s() -> string
{
    return "";
}

bool check_effaceable_header(ByteBuffer plog)
{
    auto a = plog.slice( 0, 16);
    auto b = plog.slice(16, 32);
    auto z = xor_strings(a, b);
    if (z != "ecaF")
        return false;

    auto plog_generation = plog.offset(0x38).get_uint4_le();
    cout << "Effaceable generation: " << plog_generation << endl;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////