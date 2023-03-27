/* ****************include.h************ */
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
#define WEAK __weak
#elif defined(__ICCARM__) || defined(__ICCRX__)
#define WEAK __weak
#elif defined(__GNUC__)
#define WEAK __attribute__ ((weak))
#endif

void CoAlarm(void);
void SmokeAlarm(void);
void CancelAlarm(void);
void CancelMute(void);
void CoFault(void);
void SmokeFault(void);
void CancelFault(void);
void BatteryChagne(void);
void CoPeakChange(void);
void EndLife(void);
void SelfCheckOk(res);
void MuteSuccess(void);
void MuteFail(void);
void UpdateCoPPM(void);


/* ******************source.c*********** */
WEAK void CoAlarm(void) { };

WEAK void SmokeAlarm(void) { };

WEAK void CancelAlarm(void) { };

WEAK void CancelMute(void) { };

WEAK void CoFault(void) { };

WEAK void SmokeFault(void) { };

WEAK void CancelFault(void) { };

WEAK void BatteryChagne(void) { };

WEAK void CoPeakChange(void) { };

WEAK void EndLife(void) { };

WEAK void SelfCheckOk(res) { };

WEAK void MuteSuccess(void) { };

WEAK void MuteFail(void) { };

WEAK void UpdateCoPPM(void) { };
