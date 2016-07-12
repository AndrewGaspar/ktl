
namespace ktl
{
    namespace details
    {
        template<typename Functor>
        class __declspec(code_seg(SCOPE_EXIT_CODE_SEGMENT)) SCOPE_EXIT_TYPE_NAME
        {
            Functor m_scopeExit;
            bool m_isDismissed = false;
        public:
            SCOPE_EXIT_TYPE_NAME(Functor&& f) : m_scopeExit(ktl::move(f))
            {

            }

            SCOPE_EXIT_TYPE_NAME(SCOPE_EXIT_TYPE_NAME const &) = delete;
            SCOPE_EXIT_TYPE_NAME & operator=(SCOPE_EXIT_TYPE_NAME const &) = delete;

            SCOPE_EXIT_TYPE_NAME(SCOPE_EXIT_TYPE_NAME&& other) : m_isDismissed(other.m_isDismissed), m_scopeExit(ktl::move(other.m_scopeExit))
            {
                other.m_isDismissed = true;
            }

            SCOPE_EXIT_TYPE_NAME & operator=(SCOPE_EXIT_TYPE_NAME&& other)
            {
                m_isDismissed = other.m_isDismissed;
                m_scopeExit = ktl::move(other.m_scopeExit);

                other.m_isDismissed = true;

                return *this;
            }

            void Dismiss()
            {
                m_isDismissed = true;
            }

            ~SCOPE_EXIT_TYPE_NAME()
            {
                if (!m_isDismissed)
                {
                    m_scopeExit();
                }
            }
        };
    }
}