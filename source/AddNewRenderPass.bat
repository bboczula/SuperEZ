@echo off
:: Runs the python script
cd engine/renderer
python addNewRenderPass.py

:: Pauses so the window doesn't close immediately
:: allowing you to see if it succeeded or failed.
echo.
pause