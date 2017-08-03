echo "Plotting..."
root -b -q doAll.C
root -b -q ooplt.C
echo "Plotted successfully, launching web interface."
cd webinterface
python mkjson.py
