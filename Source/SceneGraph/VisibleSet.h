/*********************************************************************
 * \file   VisibleSet.h
 * \brief  visible set class, value object
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef VISIBLE_SET_H
#define VISIBLE_SET_H

#include <vector>
#include <memory>

namespace Enigma::SceneGraph
{
    class Spatial;
    using SpatialPtr = std::shared_ptr<Spatial>;

    /** Visible object's Set */
    class VisibleSet
    {
    public:
        typedef std::vector<SpatialPtr> SpatialVector;

    public:
        VisibleSet();
        VisibleSet(const VisibleSet&) = default;
        VisibleSet(VisibleSet&&) = default;
        virtual ~VisibleSet();
        VisibleSet& operator=(const VisibleSet&) = default;
        VisibleSet& operator=(VisibleSet&&) = default;

        size_t getCount() const;
        SpatialPtr GetVisibleObject(size_t index);
        const SpatialVector& GetObjectSet() const;

        void Insert(const SpatialPtr& obj);
        void clear();

    protected:
        SpatialVector m_visibleObjSet;
    };
};

#endif // VISIBLE_SET_H
