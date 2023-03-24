#include "actions.h"
#include "web_server_itf.h"
#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

Mock<WEBSERVER_ITF> WebServerMock;
WEBSERVER_ITF*      WebServer = &WebServerMock.get();

ACTIONS* aActions;

void InitMockCalls()
{
  Fake( Method( WebServerMock, HeaterCmd ) );
}

void setUp( void )
{
  aActions = new ACTIONS( WebServer );
  WebServerMock.ClearInvocationHistory();
}

void tearDown( void )
{
  delete aActions;
}


class TEST_ACTION
{
  public:
  void TestActionHeaterOFF()
  {
    InitMockCalls();
    aActions->Execute( ACTION_HEATER_OFF_T );
    Verify( Method( WebServerMock, HeaterCmd ).Using( ACTION_HEATER_OFF_T ) );
  }

  void TestActionHeaterON()
  {
    InitMockCalls();
    aActions->Execute( ACTION_HEATER_ON_T );
    Verify( Method( WebServerMock, HeaterCmd ).Using( ACTION_HEATER_ON_T ) ).Once();
  }

  void TestActionResetDivEnerg()
  {
    Fake( Method( WebServerMock, ResetDivEnergy ) );
    aActions->Execute( ACTION_RESET_DIV_ENERGY_T );
    Verify( Method( WebServerMock, ResetDivEnergy ) ).Once();
  }

  void TestActionUpdateFirmware()
  {
    Fake( Method( WebServerMock, UpdateFirmware ) );
    aActions->Execute( ACTION_UPDATE_FIRMWARE_T );
    Verify( Method( WebServerMock, UpdateFirmware ) ).Once();
  }
};

TEST_ACTION* TestAction = new TEST_ACTION();

int main( int argc, char* argv[] )
{
  UNITY_BEGIN();
  RUN_TEST( [](){TestAction->TestActionHeaterOFF();} );
  RUN_TEST( [](){TestAction->TestActionHeaterON();} );
  RUN_TEST( [](){TestAction->TestActionResetDivEnerg();} );
  RUN_TEST( [](){TestAction->TestActionUpdateFirmware();} );
  return UNITY_END();
}