#ifndef AIDE_HIERARCHICALID_HPP
#define AIDE_HIERARCHICALID_HPP

#include <string>
#include <vector>

namespace aide
{
    class HierarchicalId
    {
    public:
        using const_iterator = std::vector<const char*>::const_iterator;

        explicit HierarchicalId(const char* level);

        HierarchicalId() = delete;

        HierarchicalId& operator()(const char* level);

        HierarchicalId operator()(const char* level) const;

        HierarchicalId& addLevel(const char* level);

        HierarchicalId addLevel(const char* level) const;

        [[nodiscard]] std::string name() const;

        bool operator==(const HierarchicalId& rhs) const;
        bool operator!=(const HierarchicalId& rhs) const;
        bool operator<(const HierarchicalId& rhs) const;
        bool operator>(const HierarchicalId& rhs) const;

        [[nodiscard]] const_iterator begin() const noexcept;

        [[nodiscard]] const_iterator end() const noexcept;

    private:
        explicit HierarchicalId(std::vector<const char*> ids);

        std::vector<const char*> m_id;
    };
} // namespace aide

#endif // AIDE_HIERARCHICALID_HPP
