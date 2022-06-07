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
  static const Rtti TYPE_RTTI; \
  virtual const Rtti& TypeInfo () const override { return TYPE_RTTI; };\
  virtual const Rtti* TypeIndex() override { return &TYPE_RTTI; };
//----------------------------------------------------------------------------
#define DECLARE_EN_RTTI \
public: \
    static const Enigma::Frameworks::Rtti TYPE_RTTI; \
    virtual const Enigma::Frameworks::Rtti& TypeInfo () const { return TYPE_RTTI; } \
    virtual const Enigma::Frameworks::Rtti* TypeIndex() { return &TYPE_RTTI; }
//----------------------------------------------------------------------------
#define IMPLEMENT_RTTI(nsname, classname, baseclassname) \
    const Enigma::Frameworks::Rtti Enigma::classname::TYPE_RTTI{ #nsname"."#classname, &baseclassname::TYPE_RTTI }
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
    class RttiRepository;

    /** Rtti class
    @remarks
    The name must be unique among all objects in the system.  In the Enigma
    namespace, a class Foo should use "En.Foo".  If an application has
    another namespace, SomeName, then the name should be "SomeName.Foo". */
    class Rtti
    {
    public:
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
        friend class RttiRepository;
        Rtti(const std::string& name);

    private:
        std::string m_name;
    };
};

#endif // !_EN_RTTI_H
