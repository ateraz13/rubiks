#ifndef ACTION_HXX
#define ACTION_HXX

class Action {
public:
  virtual void execute() = 0;
};

class QuitAction : public Action {
public:
  virtual void execute() override;
};

#endif // ACTION_HXX
