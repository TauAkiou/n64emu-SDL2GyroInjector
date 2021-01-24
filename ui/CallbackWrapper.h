// https://stackoverflow.com/questions/40326733/changing-calling-convention

template<typename Func, Func* callback>
auto make_callback()
{
    return &detail::callback_maker<Func, callback>::call;
}

template<typename T, T*>
struct callback_maker;

template<typename R, typename... Params, R(*Func)(Params...)>
struct callback_maker<R(Params...), Func>
{
    static R __stdcall call(Params... ps)
    {
        return Func(std::forward<Params>(ps)...);
    }
};