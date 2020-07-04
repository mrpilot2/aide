#ifndef AIDE_HIERARCHICALID_HPP
#define AIDE_HIERARCHICALID_HPP

#include <string>
#include <vector>

namespace aide
{
    class HierarchicalId
    {
    public:
        explicit HierarchicalId(const char* level);

        HierarchicalId() = delete;

        HierarchicalId& operator()(const char* level);

        HierarchicalId operator()(const char* level) const;

        HierarchicalId& addLevel(const char* level);

        HierarchicalId addLevel(const char* level) const;

        [[nodiscard]] std::string name() const;

    private:
        explicit HierarchicalId(std::vector<const char*> ids);

        std::vector<const char*> m_id;
    };
} // namespace aide

#endif // AIDE_HIERARCHICALID_HPP
