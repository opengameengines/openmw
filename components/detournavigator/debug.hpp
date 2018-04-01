#ifndef OPENMW_COMPONENTS_DETOURNAVIGATOR_DEBUG_H
#define OPENMW_COMPONENTS_DETOURNAVIGATOR_DEBUG_H

#include "tilebounds.hpp"

#include <components/bullethelpers/operators.hpp>
#include <components/osghelpers/operators.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class dtNavMesh;

namespace DetourNavigator
{
    inline std::ostream& operator <<(std::ostream& stream, const TileBounds& value)
    {
        return stream << "TileBounds {" << value.mMin << ", " << value.mMax << "}";
    }

    class RecastMesh;

    class Log
    {
    public:
        Log() : mEnabled(false) {}

        void setEnabled(bool value)
        {
            mEnabled = value;
        }

        bool isEnabled() const
        {
            return mEnabled;
        }

        void write(const std::string& text)
        {
            if (mEnabled)
                std::cout << text;
        }

        static Log& instance()
        {
            static Log value;
            return value;
        }

    private:
        bool mEnabled;
    };

    inline void write(std::ostream& stream)
    {
        stream << '\n';
    }

    template <class Head, class ... Tail>
    void write(std::ostream& stream, const Head& head, const Tail& ... tail)
    {
        stream << head;
        write(stream, tail ...);
    }

    template <class ... Ts>
    void log(Ts&& ... values)
    {
        auto& log = Log::instance();
        if (!log.isEnabled())
            return;
        std::ostringstream stream;
        write(stream, std::forward<Ts>(values) ...);
        log.write(stream.str());
    }

    void writeToFile(const RecastMesh& recastMesh, const std::string& pathPrefix, const std::string& revision);
    void writeToFile(const dtNavMesh& navMesh, const std::string& pathPrefix, const std::string& revision);
}

#endif
