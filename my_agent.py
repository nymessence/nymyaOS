import sys
sys.path.append('/home/erick/nymyaOS/.venv/lib/python3.13/site-packages')
# Now, import the class directly from its sub-module
from autogen.agentchat import AssistantAgent

# Your script continues here

# Define the Ollama configuration directly in the script
ollama_config_list = [
    {
        "model": "llama3:8b-instruct-q4_K_M",
        "base_url": "http://127.0.0.1:11434/v1",
        "api_type": "open_ai",
        "api_key": "ollama"
    }
]

# Create an AssistantAgent, which acts as the AI assistant.
assistant = autogen.AssistantAgent(
    name="Coder",
    system_message="You are a helpful and expert coding assistant. Reply 'TERMINATE' when the task is done and the solution is complete. You can create and edit files as needed to solve the problem.",
    llm_config={"config_list": ollama_config_list}
)

# Create a UserProxyAgent. This agent can execute code.
user_proxy = autogen.UserProxyAgent(
    name="User",
    human_input_mode="NEVER",  # Set to "NEVER" for full autonomy
    max_consecutive_auto_reply=10,
    is_termination_msg=lambda x: "TERMINATE" in x.get("content", ""),
    code_execution_config={"work_dir": "coding_sandbox", "use_docker": False}
)

# Get the task from the user
user_task = input("What task would you like the coding agent to perform? ")

# Start the chat with the user-provided task
user_proxy.initiate_chat(
    assistant,
    message=user_task
)
