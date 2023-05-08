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
#include <unordered_map>
#include <memory>
//----------------------------------------------------------------------------
#define DECLARE_RTTI \
public: \
  static Rtti TYPE_RTTI; \
  virtual const Rtti& TypeInfo () const override { return TYPE_RTTI; };
//----------------------------------------------------------------------------
#define DECLARE_EN_RTTI \
public: \
    static Enigma::Frameworks::Rtti TYPE_RTTI; \
    virtual const Enigma::Frameworks::Rtti& TypeInfo () const override { return TYPE_RTTI; } \
    virtual const Enigma::Frameworks::Rtti* TypeIndex() override { return &TYPE_RTTI; }
//----------------------------------------------------------------------------
#define DECLARE_EN_RTTI_OF_BASE \
public: \
    static Enigma::Frameworks::Rtti TYPE_RTTI; \
    virtual const Enigma::Frameworks::Rtti& TypeInfo () const { return TYPE_RTTI; } \
    virtual const Enigma::Frameworks::Rtti* TypeIndex() { return &TYPE_RTTI; }
//----------------------------------------------------------------------------
#define DECLARE_EN_RTTI_NON_BASE \
public: \
    static Enigma::Frameworks::Rtti TYPE_RTTI; \
    const Enigma::Frameworks::Rtti& TypeInfo () const { return TYPE_RTTI; } \
    const Enigma::Frameworks::Rtti* TypeIndex() { return &TYPE_RTTI; }
//----------------------------------------------------------------------------
#define DEFINE_RTTI(modulename, classname, baseclassname) \
    Enigma::Frameworks::Rtti Enigma::modulename::classname::TYPE_RTTI{"En."#modulename"."#classname, &baseclassname::TYPE_RTTI}
//----------------------------------------------------------------------------
#define DEFINE_RTTI_OF_BASE(modulename, classname) \
    Enigma::Frameworks::Rtti Enigma::modulename::classname::TYPE_RTTI{"En."#modulename"."#classname}
//----------------------------------------------------------------------------
//#define IMPLEMENT_RTTI(nsname, modulename, classname, baseclassname) \
  //  TYPE_RTTI = Enigma::Frameworks::Rtti(#nsname"."#modulename"."#classname, baseclassname::TYPE_RTTI)
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
    another namespace, SomeName, then the name should be "SomeName.Foo".
    base rtti 用指標，是base type rtti 靜態物件的位址，即使 base rtti 初始化比較晚，
    也可以正確塞入資料。
    */
    class Rtti
    {
    public:
        Rtti() : m_base(nullptr) {}
        Rtti(const std::string& name);
        Rtti(const std::string& name, const Rtti* base_rtti);
        Rtti(const Rtti& rhs) = delete;
        Rtti(Rtti&& rhs) = delete;
        ~Rtti() = default;

        Rtti& operator=(const Rtti& rhs) = delete;
        Rtti& operator=(Rtti&& rhs) = delete;
        bool operator==(const Rtti& rhs) const;

        const std::string& GetName() const;

        bool IsExactly(const Rtti& type) const;
        bool IsDerived(const Rtti& type) const;

        static bool IsDerivedFrom(const std::string& type_token, const std::string& base_rtti_token);
        static bool IsExactlyOrDerivedFrom(const std::string& type_token, const std::string& base_rtti_token);

    private:
        std::string m_name;
        const Rtti* m_base;
        static std::unique_ptr<std::unordered_map<std::string, const Rtti*>> m_valueMap; // base 的 rtti 未必比 derived 早建立，為了用name 查衍生關係，所以要建個反查表
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
