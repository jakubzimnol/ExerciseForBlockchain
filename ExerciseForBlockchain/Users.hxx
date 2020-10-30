#pragma once
#ifndef PERSON_HXX
#define PERSON_HXX

#include <string>
#include <cstddef> // std::size_t

#include <odb/core.hxx>

#pragma db object
class Users
{
public:
    Users(const std::string& email,
        const std::string& nickname,
        const std::string& password, 
        bool isActive)
        : email_(email), nickname_(nickname), password_(password), isActive_(isActive)
    {
    }

    const std::string&
        email() const
    {
        return email_;
    }

    const std::string&
        nickname() const
    {
        return nickname_;
    }

    const std::string&
        password() const
    {
        return password_;
    }


    unsigned short
        isActive() const
    {
        return isActive_;
    }

    void
        isActive(bool isActive)
    {
        isActive_ = isActive;
    }

    unsigned long id() const
    {
        return id_;
    }

private:
    friend class odb::access;

    Users() {}

#pragma db id auto
    unsigned long id_;

    std::string email_;
    std::string nickname_;
    std::string password_;
    bool isActive_;
};
#endif // PERSON_HXX