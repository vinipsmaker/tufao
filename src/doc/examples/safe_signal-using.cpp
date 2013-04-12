class OurClass: public QObject
{
    Q_OBJECT
public:
    OurClass(SomeObject *O, QObject *parent) :
        QObject(parent), O(O)
    {
        connect(&O, &SomeObject::someSignal, this, &OurClass::M);
    }

    void M()
    {
        delete O;
    }

private:
    SomeObject *O;
}
