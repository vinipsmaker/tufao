void Foobar::method()
{
    // ...

    emit conditionOneReady();

    // ...

    emit conditionTwoReady();

    // ...

    // The below signal is the only safe signal
    emit finalConditionReady();
}
