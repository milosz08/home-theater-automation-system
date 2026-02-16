import time
from tzlocal import get_localzone_name

class NtpService:
  TZ_MAP = {
    "Europe/Warsaw": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/London": "GMT0BST,M3.5.0,M10.5.0",
    "Europe/Berlin": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Paris": "CET-1CEST,M3.5.0,M10.5.0/3",
    "America/New_York": "EST5EDT,M3.2.0,M11.1.0",
    "America/Los_Angeles": "PST8PDT,M3.2.0,M11.1.0",
    "Asia/Tokyo": "JST-9",
  }

  def get_local_posix_str(self):
    try:
      iana_name = get_localzone_name()
      return self.TZ_MAP.get(iana_name, self._generate_fallback_posix())
    except Exception:
      return self._generate_fallback_posix()

  def _generate_fallback_posix(self):
    offset_hours = int(time.timezone / 3600)
    tz_abbr = time.tzname[0]
    return f"{tz_abbr}{offset_hours}"

  def get_suggested_ntp_server(self):
    return "pool.ntp.org"
