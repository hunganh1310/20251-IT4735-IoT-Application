import React, { useEffect, useState, useRef } from 'react';
import {
  Dialog,
  DialogTitle,
  DialogContent,
  IconButton,
  Box,
  Typography,
  Paper,
  FormControl,
  InputLabel,
  Select,
  MenuItem,
  TextField,
  InputAdornment,
  LinearProgress,
  Stack,
  useTheme,
  Chip
} from '@mui/material';
import CloseIcon from '@mui/icons-material/Close';
import RefreshIcon from '@mui/icons-material/Refresh';
import AccessTimeIcon from '@mui/icons-material/AccessTime';
import TimerIcon from '@mui/icons-material/Timer';
import ThermostatIcon from '@mui/icons-material/Thermostat';
import WaterIcon from '@mui/icons-material/Water';
import OpacityIcon from '@mui/icons-material/Opacity';
import {
  AreaChart,
  Area,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
} from 'recharts';
import { format } from 'date-fns';
import apiClient from '../api/axiosConfig';

interface DataPoint {
  time: string;
  ph: number;
  temperature: number;
  turbidity: number;
}

interface DeviceDashboardModalProps {
  open: boolean;
  onClose: () => void;
  deviceId: string | number;
  deviceName: string;
}

const DeviceDashboardModal: React.FC<DeviceDashboardModalProps> = ({
  open,
  onClose,
  deviceId,
  deviceName,
}) => {
  const theme = useTheme();
  const [data, setData] = useState<DataPoint[]>([]);
  const [loading, setLoading] = useState(false);
  
  // Settings state
  const [durationMinutes, setDurationMinutes] = useState<number>(30);
  const [aggregateSeconds, setAggregateSeconds] = useState<number>(10);
  const [lastUpdated, setLastUpdated] = useState<Date>(new Date());

  const intervalRef = useRef<ReturnType<typeof setInterval> | null>(null);

  const fetchData = async () => {
    if (!deviceId) return;

    try {
      const response = await apiClient.get(`/devices/${deviceId}/data`, {
        params: {
          durationMinutes,
          aggregateSeconds
        }
      });

      if (response.data && response.data.success) {
        // Lọc dữ liệu rác
        const cleanData = response.data.data.filter((item: any) => item.time && item.ph !== undefined);
        setData(cleanData);
        setLastUpdated(new Date());
      }
    } catch (error) {
      console.error("Failed to fetch dashboard data", error);
    }
  };

  useEffect(() => {
    if (open) {
      setLoading(true);
      fetchData().finally(() => setLoading(false));

      if (intervalRef.current) clearInterval(intervalRef.current);
      
      intervalRef.current = setInterval(() => {
        fetchData();
      }, aggregateSeconds * 1000);
    } else {
      if (intervalRef.current) clearInterval(intervalRef.current);
      setData([]);
    }

    return () => {
      if (intervalRef.current) clearInterval(intervalRef.current);
    };
  }, [open, deviceId, durationMinutes, aggregateSeconds]);

  const handleAggregateChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    let val = parseInt(event.target.value);
    if (isNaN(val)) val = 5;
    if (val < 5) val = 5;
    setAggregateSeconds(val);
  };

  // Custom Tooltip
  const CustomTooltip = ({ active, payload, label, unit }: any) => {
    if (active && payload && payload.length) {
      const value = Number(payload[0].value); 
      
      return (
        <Paper sx={{ p: 1.5, bgcolor: 'rgba(255, 255, 255, 0.95)', border: '1px solid #e2e8f0' }}>
          <Typography variant="caption" sx={{ color: '#64748b', display: 'block', mb: 0.5 }}>
            {format(new Date(label), 'HH:mm:ss dd/MM')}
          </Typography>
          <Typography variant="body2" sx={{ fontWeight: 700, color: payload[0].color }}>
            {!isNaN(value) ? value.toFixed(2) : '--'} {unit}
          </Typography>
        </Paper>
      );
    }
    return null;
  };

  // Component hiển thị biểu đồ
  const ChartCard = ({ title, dataKey, color, unit, icon: Icon, gradientId }: any) => {
    const lastValue = data.length > 0 ? Number(data[data.length - 1][dataKey as keyof DataPoint]) : null;

    return (
      <Paper
        elevation={0}
        sx={{
          p: 3,
          borderRadius: "24px",
          border: "1px solid #f1f5f9",
          bgcolor: "#fff",
          height: 380, 
          boxShadow: "0 4px 6px -1px rgba(0,0,0,0.05)"
        }}
      >
        <Box sx={{ display: 'flex', alignItems: 'center', mb: 2, gap: 1 }}>
          <Box sx={{ p: 1, borderRadius: "12px", bgcolor: `${color}15`, color: color }}>
            <Icon fontSize="small" />
          </Box>
          <Typography variant="h6" sx={{ fontWeight: 700, color: '#1e293b' }}>
            {title}
          </Typography>
          <Chip 
              label={lastValue !== null ? `${lastValue.toFixed(2)} ${unit}` : "--"} 
              size="small" 
              sx={{ ml: "auto", bgcolor: color, color: "#fff", fontWeight: 700 }}
          />
        </Box>
        
        {/* Tăng height của container chứa chart */}
        <Box sx={{ height: 300, width: "100%" }}>
          <ResponsiveContainer width="100%" height="100%">
            <AreaChart data={data}>
              <defs>
                <linearGradient id={gradientId} x1="0" y1="0" x2="0" y2="1">
                  <stop offset="5%" stopColor={color} stopOpacity={0.3} />
                  <stop offset="95%" stopColor={color} stopOpacity={0} />
                </linearGradient>
              </defs>
              <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="#f1f5f9" />
              <XAxis 
                dataKey="time" 
                tickFormatter={(str) => format(new Date(str), 'HH:mm:ss')}
                stroke="#94a3b8"
                fontSize={12}
                tickMargin={10}
                minTickGap={30} 
              />
              <YAxis 
                stroke="#94a3b8" 
                fontSize={12} 
                domain={['auto', 'auto']}
              />
              <Tooltip content={<CustomTooltip unit={unit} />} />
              <Area 
                type="monotone" 
                dataKey={dataKey} 
                stroke={color} 
                strokeWidth={3}
                fillOpacity={1} 
                fill={`url(#${gradientId})`} 
                animationDuration={500}
              />
            </AreaChart>
          </ResponsiveContainer>
        </Box>
      </Paper>
    );
  };

  return (
    <Dialog
      open={open}
      onClose={onClose}
      fullWidth
      maxWidth="xl"
      PaperProps={{
        sx: { borderRadius: "28px", bgcolor: "#f8fafc", minHeight: "80vh" }
      }}
    >
      {loading && <LinearProgress sx={{ position: 'absolute', top: 0, left: 0, right: 0 }} />}

      <DialogTitle sx={{ px: 4, py: 3, display: "flex", justifyContent: "space-between", alignItems: "center", bgcolor: "#fff", borderBottom: "1px solid #f1f5f9" }}>
        <Box>
          <Typography variant="h5" sx={{ fontWeight: 800, color: "#1e293b" }}>
            {deviceName} Dashboard
          </Typography>
          <Typography variant="body2" sx={{ color: "#64748b", display: 'flex', alignItems: 'center', gap: 0.5, mt: 0.5 }}>
            <span style={{ position: 'relative', display: 'flex', height: 8, width: 8 }}>
                <span style={{ animation: 'ping 1s cubic-bezier(0, 0, 0.2, 1) infinite', position: 'absolute', display: 'inline-flex', height: '100%', width: '100%', borderRadius: '50%', backgroundColor: '#22c55e', opacity: 0.75 }}></span>
                <span style={{ position: 'relative', display: 'inline-flex', borderRadius: '50%', height: 8, width: 8, backgroundColor: '#22c55e' }}></span>
            </span>
            Live monitoring • Updated: {format(lastUpdated, 'HH:mm:ss')}
            <style>{`@keyframes ping { 75%, 100% { transform: scale(2); opacity: 0; } }`}</style>
          </Typography>
        </Box>
        <IconButton onClick={onClose} sx={{ bgcolor: "#f1f5f9" }}>
          <CloseIcon />
        </IconButton>
      </DialogTitle>

      <DialogContent sx={{ p: 4 }}>
        {/* Controls Bar */}
        <Paper elevation={0} sx={{ p: 2, mb: 4, borderRadius: "16px", bgcolor: "#fff", border: "1px solid #e2e8f0", display: 'flex', flexWrap: 'wrap', gap: 3, alignItems: 'center' }}>
            {/* Giữ nguyên phần Controls */}
            <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                <Box sx={{ p: 1, borderRadius: "8px", bgcolor: "primary.lighter", color: "primary.main" }}>
                    <TimerIcon />
                </Box>
                <Typography sx={{ fontWeight: 600, color: "#475569" }}>Data Settings:</Typography>
            </Box>

            <FormControl size="small" sx={{ minWidth: 200 }}>
                <InputLabel>History Duration</InputLabel>
                <Select
                    value={durationMinutes}
                    label="History Duration"
                    onChange={(e) => setDurationMinutes(Number(e.target.value))}
                    startAdornment={<InputAdornment position="start"><AccessTimeIcon fontSize="small" /></InputAdornment>}
                    sx={{ borderRadius: "10px" }}
                >
                    <MenuItem value={30}>Last 30 Minutes</MenuItem>
                    <MenuItem value={60}>Last 1 Hour</MenuItem>
                    <MenuItem value={180}>Last 3 Hours</MenuItem>
                    <MenuItem value={360}>Last 6 Hours</MenuItem>
                    <MenuItem value={720}>Last 12 Hours</MenuItem>
                    <MenuItem value={1440}>Last 1 Day</MenuItem>
                    <MenuItem value={4320}>Last 3 Days</MenuItem>
                    <MenuItem value={10080}>Last 7 Days</MenuItem>
                </Select>
            </FormControl>

            <TextField
                label="Update Interval (Seconds)"
                type="number"
                size="small"
                value={aggregateSeconds}
                onChange={handleAggregateChange}
                InputProps={{
                    inputProps: { min: 5 },
                    startAdornment: <InputAdornment position="start"><RefreshIcon fontSize="small" /></InputAdornment>,
                }}
                helperText="Minimum 5 seconds"
                sx={{ 
                    minWidth: 200,
                    '& .MuiOutlinedInput-root': { borderRadius: "10px" }
                }}
            />

            <Box sx={{ flexGrow: 1 }} />
            
            <Chip 
                label={`${data.length} Data Points`} 
                variant="outlined" 
                size="small"
                sx={{ borderRadius: "8px", fontWeight: 600, borderColor: "#cbd5e1" }}
            />
        </Paper>

        <Stack spacing={3}>
            {/* pH Chart */}
            <ChartCard 
                title="pH Level" 
                dataKey="ph" 
                color="#8b5cf6" 
                unit="pH"
                icon={WaterIcon}
                gradientId="colorPh"
            />

            {/* Temperature Chart */}
            <ChartCard 
                title="Temperature" 
                dataKey="temperature" 
                color="#ef4444" 
                unit="°C"
                icon={ThermostatIcon}
                gradientId="colorTemp"
            />

            {/* Turbidity Chart */}
            <ChartCard 
                title="Turbidity" 
                dataKey="turbidity" 
                color="#f59e0b" 
                unit="NTU"
                icon={OpacityIcon}
                gradientId="colorTurb"
            />
        </Stack>
      </DialogContent>
    </Dialog>
  );
};

export default DeviceDashboardModal;