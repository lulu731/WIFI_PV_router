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

void Char0ShouldGetAction0()
{
  int actionType = int( '0' ) - 48;
  TEST_ASSERT_EQUAL( ACTION_HEATER_OFF_T, ACTION_TYPE( actionType ) );
}

void Char1ShouldGetAction1()
{
  int actionType = int( '1' ) - 48;
  TEST_ASSERT_EQUAL( ACTION_HEATER_ON_T, ACTION_TYPE( actionType ) );
}

void Char8ShouldGetAction8()
{
  int actionType = int( '8' ) - 48;
  TEST_ASSERT_EQUAL( ACTION_RESET_DIV_ENERGY_T, ACTION_TYPE( actionType ) );
}

void Char9ShouldGetAction9()
{
  int actionType = int( '9' ) - 48;
  TEST_ASSERT_EQUAL( ACTION_UPDATE_FIRMWARE_T, ACTION_TYPE( actionType ) );
}

int main( int argc, char* argv[] )
{
  UNITY_BEGIN();
  RUN_TEST( [](){TestAction->TestActionHeaterOFF();} );
  RUN_TEST( [](){TestAction->TestActionHeaterON();} );
  RUN_TEST( [](){TestAction->TestActionResetDivEnerg();} );
  RUN_TEST( [](){TestAction->TestActionUpdateFirmware();} );
  RUN_TEST( [](){Char0ShouldGetAction0();} );
  RUN_TEST( [](){Char1ShouldGetAction1();} );
  RUN_TEST( [](){Char8ShouldGetAction8();} );
  RUN_TEST( [](){Char9ShouldGetAction9();} );
  return UNITY_END();
}