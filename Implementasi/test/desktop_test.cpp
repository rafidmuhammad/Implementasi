#include <fsm.h>
#include <state.h>
#include <unity.h>

void test_function_fsm_init(void)
{
    int charging_state;
    fsm_init(&charging_state);
    TEST_ASSERT_EQUAL(charging_state, ON);
}

void test_function_charging_fsm_ON(void)
{
    int charging_state;
    float DayaPanel, DayaPanel_Old, ArusBaterai, TeganganBaterai, TeganganPanel, TeganganPanel_Old, duty;
    fsm_init(&charging_state);
    // ON -> OFF
    DayaPanel = 1;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(charging_state, OFF);

    // ON -> Charging
    fsm_init(&charging_state);
    DayaPanel = 10;
    TeganganBaterai = 11;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(charging_state, Charging);
    TEST_ASSERT_EQUAL(duty, 50);

    // else
    DayaPanel = 10;
    TeganganBaterai = 12.5;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(charging_state, OFF);
}

void test_function_charging_fsm_OFF(void)
{
    int charging_state;
    float DayaPanel, DayaPanel_Old, ArusBaterai, TeganganBaterai, TeganganPanel, TeganganPanel_Old, duty;
    fsm_init(&charging_state);

    // ON -> OFF
    DayaPanel = 1;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(charging_state, OFF);
    // OFF -> ON kondisi 1
    DayaPanel = 10;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(charging_state, ON);
    // ON -> OFF
    DayaPanel = 1;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(charging_state, OFF);
    // OFF -> ON kondisi 2
    TeganganBaterai = 10;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(charging_state, ON);
}

void test_function_charging_fsm_Charging(void)
{
    int charging_state;
    float DayaPanel, DayaPanel_Old, ArusBaterai, TeganganBaterai, TeganganPanel, TeganganPanel_Old, duty;
    fsm_init(&charging_state);

    // ON -> Charging
    fsm_init(&charging_state);
    DayaPanel = 10;
    TeganganBaterai = 11;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(charging_state, Charging);
    TEST_ASSERT_EQUAL(duty, 50);

    ArusBaterai = 10;
    DayaPanel = 50;
    DayaPanel_Old = 10;
    TeganganPanel = 30;
    TeganganPanel_Old = 20;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(duty, 49);
    /*
    TeganganPanel = 20;
    TeganganPanel_Old = 30;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(duty, 51);
    */
    ArusBaterai = 10;
    DayaPanel = 10;
    DayaPanel_Old = 50;
    TeganganPanel = 30;
    TeganganPanel_Old = 20;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(duty, 50);

    /*
    TeganganPanel = 20;
    TeganganPanel_Old = 30;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(duty, 48);
    */

    // Charging -> OFF
    ArusBaterai = 5;
    ChargingFSM(&charging_state, &DayaPanel, &DayaPanel_Old, &ArusBaterai, &TeganganBaterai, &TeganganPanel, &TeganganPanel_Old, &duty);
    TEST_ASSERT_EQUAL(charging_state, OFF);
}

int main(int argc, char const *argv[])
{
    UNITY_BEGIN();
    RUN_TEST(test_function_fsm_init);
    RUN_TEST(test_function_charging_fsm_ON);
    RUN_TEST(test_function_charging_fsm_OFF);
    RUN_TEST(test_function_charging_fsm_Charging);
    UNITY_END();
    return 0;
}