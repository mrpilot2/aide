#ifndef AIDE_HIERARCHICALID_HPP
#define AIDE_HIERARCHICALID_HPP

#include <string>
#include <vector>

namespace aide
{
    class HierarchicalId
    {
    public:
        explicit HierarchicalId(const char* levels);

        HierarchicalId() = delete;

        HierarchicalId& operator()(const char* level);

        HierarchicalId& addLevel(const char* level);

        [[nodiscard]] std::string name() const;

    private:
        std::vector<const char*> m_id;
    };
} // namespace aide

#endif // AIDE_HIERARCHICALID_HPP
