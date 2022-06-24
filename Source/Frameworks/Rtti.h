/*********************************************************************
 * \file   Rtti.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef _EN_RTTI_H
#define _EN_RTTI_H

#include <string>
//----------------------------------------------------------------------------
#define DECLARE_RTTI \
public: \
  static Rtti TYPE_RTTI; \
  virtual const Rtti& TypeInfo () const override { return TYPE_RTTI; };\
//----------------------------------------------------------------------------
#define DECLARE_EN_RTTI \
public: \
    static Enigma::Frameworks::Rtti TYPE_RTTI; \
    virtual const Enigma::Frameworks::Rtti& TypeInfo () const override { return TYPE_RTTI; } \
//----------------------------------------------------------------------------
#define DEFINE_RTTI(modulename, classname) \
    Enigma::Frameworks::Rtti Enigma::modulename::classname::TYPE_RTTI
//----------------------------------------------------------------------------
#define IMPLEMENT_RTTI(nsname, modulename, classname, baseclassname) \
    TYPE_RTTI = Enigma::Frameworks::Rtti(#nsname"."#modulename"."#classname, baseclassname::TYPE_RTTI)
//----------------------------------------------------------------------------
#define IMPLEMENT_TEMPLATE_RTTI(nsname, classname, baseclassname) \
  template <> \
  const Enigma::Frameworks::Rtti classname::TYPE_RTTI{ #nsname"."#classname, &baseclassname::TYPE_RTTI }
//----------------------------------------------------------------------------
#define IMPLEMENT_EN_TEMPLATE_RTTI(nsname, classname, baseclassname) \
    template <> \
    const Enigma::Frameworks::Rtti classname::TYPE_RTTI{ #nsname"."#classname, &baseclassname::TYPE_RTTI }
//----------------------------------------------------------------------------

namespace Enigma::Frameworks
{
    /** Rtti class
    @remarks
    The name must be unique among all objects in the system.  In the Enigma
    namespace, a class Foo should use "En.Foo".  If an application has
    another namespace, SomeName, then the name should be "SomeName.Foo". */
    class Rtti
    {
    public:
        Rtti() {}
        Rtti(const std::string& name);
        Rtti(const std::string& name, const Rtti& base_rtti);
        Rtti(const Rtti& rhs) = default;
        Rtti(Rtti&& rhs) = default;
        ~Rtti() = default;

        Rtti& operator=(const Rtti& rhs) = default;
        Rtti& operator=(Rtti&& rhs) = default;
        bool operator==(const Rtti& rhs) const;

        const std::string& GetName() const;

        bool IsExactly(const Rtti& type) const;
        bool IsDerived(const Rtti& type) const;

    private:
        std::string m_name;
    };
    class RttiHashFunc
    {
    public:
        size_t operator()(const Rtti& rtti) const
        {
            return std::hash<std::string>()(rtti.GetName());
        }
    };
};

#endif // !_EN_RTTI_H
