# Security Policy

## Scope

This is a local single-player desktop game with no network features. The attack surface is limited to file I/O (save data, config) and any future asset loading.

## Rules

### File I/O
- Never construct file paths from user-supplied strings without sanitising for path traversal (`..`, absolute paths, null bytes)
- Save/config files are written to a fixed known location only — no user-controlled paths
- Validate all data read from disk before use (bounds-check scores, grid sizes, etc.) — treat save files as untrusted input

### Asset loading
- Assets are bundled with the game and loaded from a fixed relative path — do not load assets from arbitrary user-supplied paths
- If texture/audio loading fails, fail gracefully rather than crashing on a null pointer

### No code execution
- Do not eval, exec, or shell-out based on any runtime data
- No scripting engine, no dynamic library loading unless explicitly planned and reviewed

### Secrets
- Never commit or add secrets of any kind to the repository — no API keys, passwords, tokens, or credentials
- If a secret is ever needed (e.g. a future leaderboard API key), use an environment variable or a local-only config file excluded via .gitignore

### Future network features
- Any multiplayer or leaderboard feature must be reviewed before implementation — do not add network code speculatively
