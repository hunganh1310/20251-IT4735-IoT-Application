

import { useState, useEffect } from "react";
import { 
    Box, 
    Button, 
    Dialog, 
    DialogActions, 
    DialogContent, 
    DialogTitle, 
    TextField, 
    Typography,
    Paper,
    InputAdornment,
    Fade,
    IconButton,
    Grid,
    Card,
    Chip,
    CircularProgress,
    Tooltip,
    Stack,
    Divider,
    Zoom,
    Switch,
    Slider,
    FormControl,
    InputLabel,
    Select,
    MenuItem,
    FormControlLabel,
    GlobalStyles
} from "@mui/material";
import AddIcon from "@mui/icons-material/Add";
import DevicesIcon from "@mui/icons-material/Devices";
import LabelIcon from "@mui/icons-material/Label";
import LightbulbIcon from "@mui/icons-material/Lightbulb";
import CloseIcon from "@mui/icons-material/Close";
import DeleteIcon from "@mui/icons-material/Delete";
import EditIcon from "@mui/icons-material/Edit";
import RefreshIcon from "@mui/icons-material/Refresh";
import SettingsRemoteIcon from "@mui/icons-material/SettingsRemote";
import RouterIcon from "@mui/icons-material/Router";
import WarningAmberIcon from "@mui/icons-material/WarningAmber";
import PaletteIcon from "@mui/icons-material/Palette";
import TuneIcon from "@mui/icons-material/Tune";
import { deviceApi } from "../api/deviceApi";
import { ledApi } from "../api/ledApi";
import { toast } from "react-toastify";

const modeLabels: Record<string, string> = {
    basic: "Static Color",
    sky_simulation: "Sky Simulation",
    rain: "Rain Effect",
    meteor: "Meteor Shower",
    apocalypse: "Apocalypse Mode"
};

const getModeBackground = (mode: string, isOn: boolean, color: string) => {
    if (!isOn) return { bgcolor: "#fff", color: "inherit" };
    
    switch (mode) {
        case "sky_simulation":
            return {
                background: "linear-gradient(135deg, #e0f2fe 0%, #7dd3fc 100%)",
                color: "#0369a1",
                borderColor: "#7dd3fc"
            };
        case "rain":
            return {
                background: "linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)), url('https://media0.giphy.com/media/v1.Y2lkPWVjZjA1ZTQ3Zml2cXVvMjVvYms0MmthN29ybzgxeWtrYTdmZnRzY2t2bmU0cWV5bCZlcD12MV9naWZzX3NlYXJjaCZjdD1n/xTcnT45z6H5gxFYZZS/200.webp')",
                backgroundSize: "cover",
                backgroundPosition: "center",
                color: "#fff",
                borderColor: "#334155",
                "& .MuiTypography-root": { color: "#fff" },
                "& .MuiTypography-caption": { color: "rgba(255, 255, 255, 0.7)" },
                "& .MuiSvgIcon-root": { color: "#fff" }
            };
        case "meteor":
            return {
                background: "#020617",
                position: "relative",
                overflow: "hidden",
                color: "#e0e7ff",
                borderColor: "#1e1b4b",
                "&::before": {
                    content: '""',
                    position: "absolute",
                    top: "-100%",
                    left: "-100%",
                    right: "-100%",
                    bottom: "-100%",
                    background: `
                        radial-gradient(1px 1px at 20px 30px, #fff, rgba(0,0,0,0)),
                        radial-gradient(1px 1px at 40px 70px, #fff, rgba(0,0,0,0)),
                        radial-gradient(1px 1px at 50px 160px, #fff, rgba(0,0,0,0)),
                        radial-gradient(2px 2px at 80px 120px, #fff, rgba(0,0,0,0)),
                        radial-gradient(1px 1px at 110px 50px, #fff, rgba(0,0,0,0)),
                        radial-gradient(2px 2px at 150px 180px, #fff, rgba(0,0,0,0)),
                        radial-gradient(1px 1px at 180px 40px, #fff, rgba(0,0,0,0)),
                        radial-gradient(1px 1px at 220px 100px, #fff, rgba(0,0,0,0))
                    `,
                    backgroundSize: "250px 250px",
                    animation: "meteorAnimation 1.5s linear infinite",
                    opacity: 0.6,
                    zIndex: 0
                },
                "&::after": {
                    content: '""',
                    position: "absolute",
                    top: 0,
                    left: 0,
                    width: "100%",
                    height: "100%",
                    background: "linear-gradient(45deg, rgba(255,255,255,0.1) 0%, rgba(255,255,255,0) 100%)",
                    zIndex: 1
                },
                "& > *": { position: "relative", zIndex: 2 },
                "& .MuiTypography-root": { color: "#e0e7ff" },
                "& .MuiTypography-caption": { color: "rgba(224, 231, 255, 0.7)" },
                "& .MuiSvgIcon-root": { color: "#e0e7ff" }
            };
        case "apocalypse":
            return {
                background: "linear-gradient(135deg, #450a0a 0%, #991b1b 100%)",
                color: "#fef2f2",
                borderColor: "#b91c1c",
                "& .MuiTypography-root": { color: "#fef2f2" },
                "& .MuiTypography-caption": { color: "rgba(254, 242, 242, 0.7)" },
                "& .MuiSvgIcon-root": { color: "#fef2f2" }
            };
        case "basic":
            // Lighten the hex color for background
            return {
                background: `linear-gradient(135deg, #fff 0%, ${color}22 100%)`,
                borderColor: color,
                color: "inherit"
            };
        default:
            return { bgcolor: "#fff", color: "inherit" };
    }
};

const HomePage = () => {
    const [devices, setDevices] = useState<any[]>([]);
    const [open, setOpen] = useState(false);
    const [ledDialogOpen, setLedDialogOpen] = useState(false);
    const [deleteConfirmOpen, setDeleteConfirmOpen] = useState(false);
    const [deleteTarget, setDeleteTarget] = useState<any>(null);
    const [loading, setLoading] = useState(false);
    const [fetching, setFetching] = useState(true);
    const [editId, setEditId] = useState<number | null>(null);
    const [selectedDevice, setSelectedDevice] = useState<any>(null);
    
    const [formData, setFormData] = useState({
        deviceId: "",
        name: "",
        nameLed: ""
    });

    const [ledData, setLedData] = useState({
        name: "",
        led_mode: "basic",
        brightness: 0,
        led_is_on: false,
        presence_mode_enabled: false,
        color: "#FF0000",
        deviceId: 0
    });

    const user = JSON.parse(localStorage.getItem("user") || sessionStorage.getItem("user") || "{}");

    const fetchDevices = async () => {
        setFetching(true);
        try {
            const response = await deviceApi.getAllDevices();
            setDevices(response.data.data || []);
        } catch (error: any) {
            toast.error("Failed to fetch devices", {
                position: "top-right",
                autoClose: 3000,
            });
        } finally {
            setFetching(false);
        }
    };

    useEffect(() => {
        fetchDevices();
    }, []);

    const handleOpen = (device?: any) => {
        if (device) {
            setEditId(device.id);
            setFormData({
                deviceId: device.deviceId,
                name: device.name || "",
                nameLed: device.led?.name || ""
            });
        } else {
            setEditId(null);
            setFormData({ deviceId: "", name: "", nameLed: "" });
        }
        setOpen(true);
    };

    const handleLedOpen = (device: any) => {
        setSelectedDevice(device);
        if (device.led) {
            setLedData({
                name: device.led.name,
                led_mode: device.led.led_mode || "basic",
                brightness: device.led.brightness || 0,
                led_is_on: device.led.led_is_on || false,
                presence_mode_enabled: device.led.presence_mode_enabled || false,
                color: device.led.color || "#FF0000",
                deviceId: device.id
            });
        }
        setLedDialogOpen(true);
    };

    const handleClose = () => {
        setOpen(false);
        setEditId(null);
        setFormData({ deviceId: "", name: "", nameLed: "" });
    };

    const handleLedClose = () => {
        setLedDialogOpen(false);
        setSelectedDevice(null);
    };

    const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
        setFormData({ ...formData, [e.target.name]: e.target.value });
    };

    const handleLedChange = (e: any) => {
        const { name, value, checked, type } = e.target;
        setLedData({ 
            ...ledData, 
            [name]: type === 'checkbox' ? checked : value 
        });
    };

    const handleSliderChange = (_: any, newValue: number | number[]) => {
        setLedData({ ...ledData, brightness: newValue as number });
    };

    const handleSubmit = async () => {
        if (!formData.deviceId) {
            toast.error("Device ID is required");
            return;
        }

        setLoading(true);
        try {
            if (editId) {
                await deviceApi.updateDevice(editId, {
                    ...formData,
                    accountId: user.id
                });
                toast.success("Device updated successfully! ✨");
            } else {
                await deviceApi.createDevice({
                    ...formData,
                    accountId: user.id
                });
                toast.success("New device added successfully! 🚀");
            }
            handleClose();
            fetchDevices();
        } catch (error: any) {
            toast.error(error.response?.data?.message || "Operation failed ❌");
        } finally {
            setLoading(false);
        }
    };

    const handleLedSubmit = async () => {
        if (!selectedDevice?.led?.id) return;
        
        setLoading(true);
        try {
            await ledApi.updateLed(selectedDevice.led.id, ledData);
            toast.success("LED settings updated! 💡");
            handleLedClose();
            fetchDevices();
        } catch (error: any) {
            toast.error("Failed to update LED settings ❌");
        } finally {
            setLoading(false);
        }
    };

    const handleTogglePresenceMode = async (e: React.MouseEvent, device: any) => {
        e.stopPropagation();
        if (!device.led?.id) return;

        try {
            const newStatus = !device.led.presence_mode_enabled;
            await ledApi.updateLed(device.led.id, {
                name: device.led.name,
                led_mode: device.led.led_mode,
                brightness: device.led.brightness,
                led_is_on: device.led.led_is_on,
                presence_mode_enabled: newStatus,
                color: device.led.color,
                deviceId: device.id
            });
            toast.success(`Auto Mode ${newStatus ? "Enabled" : "Disabled"}! 🤖`);
            fetchDevices();
        } catch (error: any) {
            toast.error("Failed to toggle Auto Mode ❌");
        }
    };

    const handleDeleteClick = (device: any) => {
        setDeleteTarget(device);
        setDeleteConfirmOpen(true);
    };

    const handleDeleteConfirm = async () => {
        if (!deleteTarget) return;
        
        setLoading(true);
        try {
            await deviceApi.deleteDevice(deleteTarget.id);
            toast.success(`Device "${deleteTarget.name || deleteTarget.deviceId}" deleted successfully 🗑️`);
            setDeleteConfirmOpen(false);
            setDeleteTarget(null);
            fetchDevices();
        } catch (error: any) {
            toast.error("Failed to delete device ❌");
        } finally {
            setLoading(false);
        }
    };

    return (
        <Box sx={{ p: { xs: 2, md: 4 }, maxWidth: 1200, margin: "0 auto" }}>
            <GlobalStyles
                styles={{
                    "@keyframes meteorAnimation": {
                        "0%": {
                            transform: "translate(0, 0) rotate(45deg)",
                            boxShadow: "0 0 0 rgba(255,255,255,0)"
                        },
                        "50%": {
                            boxShadow: "0 0 20px rgba(255,255,255,0.4)"
                        },
                        "100%": {
                            transform: "translate(300px, 300px) rotate(45deg)",
                            boxShadow: "0 0 0 rgba(255,255,255,0)"
                        }
                    }
                }}
            />
            {/* Header Section */}
            <Box sx={{ 
                display: "flex", 
                flexDirection: { xs: "column", sm: "row" },
                justifyContent: "space-between", 
                alignItems: { xs: "flex-start", sm: "center" }, 
                mb: 5,
                gap: 2
            }}>
                <Box>
                    <Typography variant="h4" sx={{ fontWeight: 800, color: "#1e293b", letterSpacing: "-0.5px" }}>
                        Device Management
                    </Typography>
                    <Typography variant="body1" sx={{ color: "#64748b", mt: 0.5 }}>
                        You have <Box component="span" sx={{ color: "primary.main", fontWeight: 700 }}>{devices.length}</Box> active devices
                    </Typography>
                </Box>
                <Stack direction="row" spacing={2}>
                    <Tooltip title="Refresh Data">
                        <IconButton 
                            onClick={fetchDevices} 
                            disabled={fetching}
                            sx={{ bgcolor: "#f1f5f9", "&:hover": { bgcolor: "#e2e8f0" } }}
                        >
                            <RefreshIcon sx={{ color: "#475569" }} />
                        </IconButton>
                    </Tooltip>
                    <Button 
                        variant="contained" 
                        startIcon={<AddIcon />} 
                        onClick={() => handleOpen()}
                        sx={{ 
                            borderRadius: "12px",
                            px: 3,
                            py: 1.2,
                            textTransform: "none",
                            fontSize: "0.95rem",
                            fontWeight: 600,
                            boxShadow: "0 10px 15px -3px rgba(59, 130, 246, 0.3)",
                        }}
                    >
                        Add New Device
                    </Button>
                </Stack>
            </Box>

            {fetching ? (
                <Box sx={{ display: "flex", flexDirection: "column", alignItems: "center", py: 15, gap: 2 }}>
                    <CircularProgress size={40} thickness={4} />
                    <Typography sx={{ color: "#94a3b8", fontWeight: 500 }}>Loading your devices...</Typography>
                </Box>
            ) : devices.length === 0 ? (
                <Fade in={true}>
                    <Paper 
                        sx={{ 
                            p: 10, 
                            textAlign: "center", 
                            borderRadius: "32px",
                            bgcolor: "#f8fafc",
                            border: "2px dashed #e2e8f0",
                            boxShadow: "none"
                        }}
                    >
                        <Box sx={{ 
                            width: 80, 
                            height: 80, 
                            borderRadius: "24px", 
                            bgcolor: "#fff", 
                            display: "flex", 
                            alignItems: "center", 
                            justifyContent: "center",
                            margin: "0 auto 24px",
                            boxShadow: "0 4px 6px -1px rgba(0,0,0,0.1)"
                        }}>
                            <RouterIcon sx={{ fontSize: 40, color: "#94a3b8" }} />
                        </Box>
                        <Typography variant="h5" sx={{ color: "#1e293b", fontWeight: 700, mb: 1 }}>
                            No devices connected
                        </Typography>
                        <Typography variant="body1" sx={{ color: "#64748b", mb: 4, maxWidth: 400, mx: "auto" }}>
                            Get started by adding your first IoT device to monitor real-time data and control peripherals.
                        </Typography>
                        <Button 
                            variant="outlined" 
                            onClick={() => handleOpen()}
                            sx={{ borderRadius: "12px", px: 4, py: 1, borderWidth: 2, "&:hover": { borderWidth: 2 } }}
                        >
                            Register Device
                        </Button>
                    </Paper>
                </Fade>
            ) : (
                <Stack spacing={2.5}>
                    {devices.map((device) => (
                        <Fade in={true} key={device.id}>
                            <Card 
                                sx={{ 
                                    borderRadius: "24px",
                                    p: 0,
                                    overflow: "hidden",
                                    border: "1px solid #f1f5f9",
                                    boxShadow: "0 1px 3px rgba(0,0,0,0.05)",
                                    transition: "all 0.4s cubic-bezier(0.4, 0, 0.2, 1)",
                                    cursor: "pointer",
                                    position: "relative",
                                    ...getModeBackground(device.led?.led_mode, device.led?.led_is_on, device.led?.color || "#FF0000"),
                                    "&:hover": {
                                        transform: "translateX(8px)",
                                        boxShadow: "0 20px 25px -5px rgba(0,0,0,0.1), 0 10px 10px -5px rgba(0,0,0,0.04)",
                                        borderColor: "primary.light"
                                    }
                                }}
                                onClick={() => handleLedOpen(device)}
                            >
                                <Stack 
                                    direction={{ xs: "column", md: "row" }} 
                                    alignItems={{ xs: "flex-start", md: "center" }}
                                    sx={{ p: 3, gap: 3 }}
                                >
                                    {/* Icon & Status */}
                                    <Box sx={{ 
                                        width: 64, 
                                        height: 64, 
                                        borderRadius: "20px", 
                                        bgcolor: device.led?.led_is_on ? "rgba(255,255,255,0.2)" : "primary.lighter", 
                                        color: device.led?.led_is_on ? "#fff" : "primary.main",
                                        display: "flex",
                                        alignItems: "center",
                                        justifyContent: "center",
                                        flexShrink: 0,
                                        backdropFilter: device.led?.led_is_on ? "blur(4px)" : "none",
                                        border: device.led?.led_is_on ? "1px solid rgba(255,255,255,0.3)" : "none"
                                    }}>
                                        <SettingsRemoteIcon sx={{ fontSize: 32 }} />
                                    </Box>

                                    {/* Device Info */}
                                    <Box sx={{ flexGrow: 1 }}>
                                        <Stack direction="row" alignItems="center" spacing={1.5} sx={{ mb: 0.5 }}>
                                            <Typography variant="h6" sx={{ fontWeight: 700 }}>
                                                {device.name || "Unnamed Device"}
                                            </Typography>
                                            <Chip 
                                                label={device.led?.led_is_on ? "LED ON" : "LED OFF"} 
                                                size="small" 
                                                sx={{ 
                                                    bgcolor: device.led?.led_is_on ? "rgba(255,255,255,0.9)" : "#f1f5f9", 
                                                    color: device.led?.led_is_on ? "#059669" : "#64748b", 
                                                    fontWeight: 800,
                                                    fontSize: "0.7rem",
                                                    height: 20,
                                                    boxShadow: device.led?.led_is_on ? "0 2px 4px rgba(0,0,0,0.1)" : "none"
                                                }} 
                                            />
                                        </Stack>
                                        <Stack direction="row" spacing={3} divider={<Divider orientation="vertical" flexItem sx={{ height: 16, my: "auto", bgcolor: device.led?.led_is_on ? "rgba(255,255,255,0.3)" : "divider" }} />}>
                                            <Typography variant="body2" sx={{ display: "flex", alignItems: "center", gap: 0.5, opacity: 0.9 }}>
                                                <Box component="span" sx={{ fontWeight: 600 }}>ID:</Box> {device.deviceId}
                                            </Typography>
                                            <Typography variant="body2" sx={{ display: "flex", alignItems: "center", gap: 0.5, opacity: 0.9 }}>
                                                <LightbulbIcon sx={{ fontSize: 16, color: device.led?.led_is_on ? "#fbbf24" : "text.disabled" }} />
                                                <Box component="span" sx={{ fontWeight: 600 }}>LED:</Box> {device.led?.name || "Not set"}
                                            </Typography>
                                            <Typography variant="body2" sx={{ display: "flex", alignItems: "center", gap: 0.5, opacity: 0.9 }}>
                                                <TuneIcon sx={{ fontSize: 16, color: device.led?.led_is_on ? "#fff" : "primary.main" }} />
                                                <Box component="span" sx={{ fontWeight: 600 }}>Mode:</Box> {modeLabels[device.led?.led_mode] || device.led?.led_mode || "Static Color"}
                                            </Typography>
                                        </Stack>
                                    </Box>

                                    {/* Actions */}
                                    <Stack direction="row" spacing={1.5} sx={{ width: { xs: "100%", md: "auto" }, justifyContent: "flex-end", alignItems: "center" }}>
                                        <FormControlLabel
                                            control={
                                                <Switch 
                                                    checked={device.led?.presence_mode_enabled || false}
                                                    onChange={(e) => handleTogglePresenceMode(e as any, device)}
                                                    color="primary"
                                                    size="small"
                                                    sx={{
                                                        '& .MuiSwitch-switchBase.Mui-checked': {
                                                            color: device.led?.led_is_on ? "#fff" : "primary.main",
                                                        },
                                                        '& .MuiSwitch-switchBase.Mui-checked + .MuiSwitch-track': {
                                                            backgroundColor: device.led?.led_is_on ? "rgba(255,255,255,0.5)" : "primary.main",
                                                        },
                                                    }}
                                                />
                                            }
                                            label={
                                                <Typography variant="caption" sx={{ 
                                                    fontWeight: 700, 
                                                    color: device.led?.led_is_on ? "#fff" : "#64748b",
                                                    mr: 1
                                                }}>
                                                    AUTO
                                                </Typography>
                                            }
                                            labelPlacement="start"
                                            sx={{ m: 0 }}
                                            onClick={(e) => e.stopPropagation()}
                                        />
                                        <Tooltip title="Edit Device">
                                            <IconButton 
                                                onClick={(e) => { e.stopPropagation(); handleOpen(device); }}
                                                sx={{ 
                                                    color: device.led?.led_is_on ? "#fff" : "#64748b", 
                                                    bgcolor: device.led?.led_is_on ? "rgba(255,255,255,0.1)" : "#f8fafc", 
                                                    "&:hover": { bgcolor: device.led?.led_is_on ? "rgba(255,255,255,0.2)" : "primary.lighter" } 
                                                }}
                                            >
                                                <EditIcon fontSize="small" />
                                            </IconButton>
                                        </Tooltip>
                                        <Tooltip title="Delete Device">
                                            <IconButton 
                                                onClick={(e) => { e.stopPropagation(); handleDeleteClick(device); }}
                                                sx={{ 
                                                    color: device.led?.led_is_on ? "#fff" : "#64748b", 
                                                    bgcolor: device.led?.led_is_on ? "rgba(255,255,255,0.1)" : "#f8fafc", 
                                                    "&:hover": { color: "#ef4444", bgcolor: device.led?.led_is_on ? "rgba(239,68,68,0.2)" : "#fef2f2" } 
                                                }}
                                            >
                                                <DeleteIcon fontSize="small" />
                                            </IconButton>
                                        </Tooltip>
                                        <Button 
                                            variant={device.led?.led_is_on ? "contained" : "outlined"}
                                            size="small"
                                            onClick={(e) => { e.stopPropagation(); toast.info("Dashboard feature coming soon! 📊"); }}
                                            sx={{ 
                                                borderRadius: "10px", 
                                                px: 2, 
                                                fontWeight: 700,
                                                textTransform: "none",
                                                borderColor: device.led?.led_is_on ? "transparent" : "#e2e8f0",
                                                bgcolor: device.led?.led_is_on ? "rgba(255,255,255,0.9)" : "transparent",
                                                color: device.led?.led_is_on ? "primary.main" : "#475569",
                                                "&:hover": {
                                                    bgcolor: device.led?.led_is_on ? "#fff" : "transparent",
                                                    borderColor: device.led?.led_is_on ? "transparent" : "primary.main"
                                                }
                                            }}
                                        >
                                            View Dashboard
                                        </Button>
                                    </Stack>
                                </Stack>
                            </Card>
                        </Fade>
                    ))}
                </Stack>
            )}

            {/* Add/Edit Dialog */}
            <Dialog 
                open={open} 
                onClose={handleClose} 
                fullWidth 
                maxWidth="xs"
                PaperProps={{
                    sx: { borderRadius: "28px", p: 1 }
                }}
            >
                <DialogTitle sx={{ px: 3, pt: 3, pb: 1, display: "flex", justifyContent: "space-between", alignItems: "center" }}>
                    <Typography variant="h5" sx={{ fontWeight: 800, color: "#1e293b" }}>
                        {editId ? "Edit Device" : "New Device"}
                    </Typography>
                    <IconButton onClick={handleClose} size="small" sx={{ bgcolor: "#f1f5f9" }}>
                        <CloseIcon fontSize="small" />
                    </IconButton>
                </DialogTitle>
                <DialogContent sx={{ px: 3 }}>
                    <Typography variant="body2" sx={{ color: "#64748b", mb: 4 }}>
                        {editId ? "Update your device configuration below." : "Fill in the details to register your new IoT device."}
                    </Typography>
                    <Stack spacing={3}>
                        <TextField
                            fullWidth
                            label="Device ID"
                            name="deviceId"
                            value={formData.deviceId}
                            onChange={handleChange}
                            disabled={!!editId}
                            placeholder="e.g. esp32-main-01"
                            InputProps={{
                                startAdornment: <InputAdornment position="start"><DevicesIcon sx={{ color: "primary.main", fontSize: 20 }} /></InputAdornment>,
                                sx: { borderRadius: "16px", bgcolor: "#f8fafc" }
                            }}
                        />
                        <TextField
                            fullWidth
                            label="Display Name"
                            name="name"
                            value={formData.name}
                            onChange={handleChange}
                            placeholder="e.g. Living Room Hub"
                            InputProps={{
                                startAdornment: <InputAdornment position="start"><LabelIcon sx={{ color: "primary.main", fontSize: 20 }} /></InputAdornment>,
                                sx: { borderRadius: "16px", bgcolor: "#f8fafc" }
                            }}
                        />
                        <TextField
                            fullWidth
                            label="LED Identifier"
                            name="nameLed"
                            value={formData.nameLed}
                            onChange={handleChange}
                            placeholder="e.g. status_led"
                            InputProps={{
                                startAdornment: <InputAdornment position="start"><LightbulbIcon sx={{ color: "primary.main", fontSize: 20 }} /></InputAdornment>,
                                sx: { borderRadius: "16px", bgcolor: "#f8fafc" }
                            }}
                        />
                    </Stack>
                </DialogContent>
                <DialogActions sx={{ p: 3, pt: 1 }}>
                    <Button 
                        fullWidth
                        onClick={handleSubmit} 
                        variant="contained" 
                        disabled={loading}
                        sx={{ 
                            borderRadius: "16px",
                            py: 1.5,
                            fontWeight: 700,
                            fontSize: "1rem",
                            textTransform: "none",
                            boxShadow: "0 10px 15px -3px rgba(59, 130, 246, 0.3)",
                        }}
                    >
                        {loading ? <CircularProgress size={24} color="inherit" /> : (editId ? "Save Changes" : "Create Device")}
                    </Button>
                </DialogActions>
            </Dialog>

            {/* LED Settings Dialog */}
            <Dialog 
                open={ledDialogOpen} 
                onClose={handleLedClose}
                TransitionComponent={Zoom}
                PaperProps={{
                    sx: { borderRadius: "28px", width: "100%", maxWidth: 450, p: 1 }
                }}
            >
                <DialogTitle sx={{ px: 3, pt: 3, pb: 1, display: "flex", justifyContent: "space-between", alignItems: "center" }}>
                    <Box sx={{ display: 'flex', alignItems: 'center', gap: 1.5 }}>
                        <Box sx={{ 
                            width: 40, 
                            height: 40, 
                            borderRadius: "12px", 
                            bgcolor: "primary.lighter", 
                            color: "primary.main",
                            display: "flex",
                            alignItems: "center",
                            justifyContent: "center"
                        }}>
                            <PaletteIcon />
                        </Box>
                        <Typography variant="h5" sx={{ fontWeight: 800, color: "#1e293b" }}>
                            LED Settings
                        </Typography>
                    </Box>
                    <IconButton onClick={handleLedClose} size="small" sx={{ bgcolor: "#f1f5f9" }}>
                        <CloseIcon fontSize="small" />
                    </IconButton>
                </DialogTitle>
                <DialogContent sx={{ px: 3 }}>
                    <Typography variant="body2" sx={{ color: "#64748b", mb: 4 }}>
                        Control <strong>{selectedDevice?.name}</strong>'s LED state, brightness, and effects.
                    </Typography>
                    
                    <Stack spacing={4}>
                        <Box sx={{ 
                            display: 'flex', 
                            justifyContent: 'space-between', 
                            alignItems: 'center',
                            p: 2,
                            borderRadius: "16px",
                            bgcolor: ledData.led_is_on ? "warning.lighter" : "#f8fafc",
                            transition: "all 0.3s ease"
                        }}>
                            <Box sx={{ display: 'flex', alignItems: 'center', gap: 1.5 }}>
                                <LightbulbIcon color={ledData.led_is_on ? "warning" : "disabled"} />
                                <Typography sx={{ fontWeight: 700, color: "#1e293b" }}>Power Status</Typography>
                            </Box>
                            <Switch 
                                name="led_is_on"
                                checked={ledData.led_is_on}
                                onChange={handleLedChange}
                                color="warning"
                            />
                        </Box>

                        <Box sx={{ 
                            display: 'flex', 
                            justifyContent: 'space-between', 
                            alignItems: 'center',
                            p: 2,
                            borderRadius: "16px",
                            bgcolor: ledData.presence_mode_enabled ? "primary.lighter" : "#f8fafc",
                            transition: "all 0.3s ease"
                        }}>
                            <Box sx={{ display: 'flex', alignItems: 'center', gap: 1.5 }}>
                                <SettingsRemoteIcon color={ledData.presence_mode_enabled ? "primary" : "disabled"} />
                                <Typography sx={{ fontWeight: 700, color: "#1e293b" }}>Auto Mode (Radar)</Typography>
                            </Box>
                            <Switch 
                                name="presence_mode_enabled"
                                checked={ledData.presence_mode_enabled}
                                onChange={handleLedChange}
                                color="primary"
                            />
                        </Box>

                        <Box sx={{ px: 1 }}>
                            <Typography gutterBottom sx={{ fontWeight: 700, color: "#1e293b", display: 'flex', alignItems: 'center', gap: 1, mb: 2 }}>
                                <TuneIcon fontSize="small" color="primary" /> Brightness ({ledData.brightness}%)
                            </Typography>
                            <Slider
                                value={ledData.brightness}
                                onChange={handleSliderChange}
                                valueLabelDisplay="auto"
                                sx={{ 
                                    color: "primary.main",
                                    '& .MuiSlider-thumb': {
                                        width: 24,
                                        height: 24,
                                        backgroundColor: '#fff',
                                        border: '2px solid currentColor',
                                        '&:focus, &:hover, &.Mui-active, &.Mui-focusVisible': {
                                            boxShadow: 'inherit',
                                        },
                                    },
                                }}
                            />
                        </Box>

                        <FormControl fullWidth>
                            <InputLabel sx={{ fontWeight: 600 }}>LED Mode</InputLabel>
                            <Select
                                name="led_mode"
                                value={ledData.led_mode}
                                label="LED Mode"
                                onChange={handleLedChange}
                                sx={{ borderRadius: "16px", bgcolor: "#f8fafc" }}
                            >
                                <MenuItem value="basic">Static Color</MenuItem>
                                <MenuItem value="sky_simulation">Sky Simulation</MenuItem>
                                <MenuItem value="rain">Rain Effect</MenuItem>
                                <MenuItem value="meteor">Meteor Shower</MenuItem>
                                <MenuItem value="apocalypse">Apocalypse Mode</MenuItem>
                            </Select>
                        </FormControl>

                        <Box sx={{ px: 1 }}>
                            <Typography gutterBottom sx={{ fontWeight: 700, color: "#1e293b", mb: 2 }}>LED Color</Typography>
                            <Box sx={{ display: 'flex', gap: 2, alignItems: 'center' }}>
                                <Box sx={{ 
                                    position: 'relative',
                                    width: 64,
                                    height: 64,
                                    borderRadius: "16px",
                                    overflow: 'hidden',
                                    border: '4px solid #fff',
                                    boxShadow: "0 4px 12px rgba(0,0,0,0.1)"
                                }}>
                                    <input
                                        type="color"
                                        name="color"
                                        value={ledData.color}
                                        onChange={handleLedChange}
                                        style={{
                                            position: 'absolute',
                                            top: -10,
                                            left: -10,
                                            width: 100,
                                            height: 100,
                                            border: 'none',
                                            cursor: 'pointer',
                                            padding: 0
                                        }}
                                    />
                                </Box>
                                <Box>
                                    <Typography variant="body2" sx={{ 
                                        fontFamily: 'monospace', 
                                        fontWeight: 700,
                                        color: "primary.main",
                                        bgcolor: "primary.lighter", 
                                        px: 1.5, 
                                        py: 0.5, 
                                        borderRadius: "8px" 
                                    }}>
                                        {ledData.color.toUpperCase()}
                                    </Typography>
                                    <Typography variant="caption" color="text.secondary">
                                        Click the box to change color
                                    </Typography>
                                </Box>
                            </Box>
                        </Box>
                    </Stack>
                </DialogContent>
                <DialogActions sx={{ p: 3, pt: 1 }}>
                    <Button 
                        fullWidth
                        onClick={handleLedSubmit} 
                        variant="contained" 
                        disabled={loading}
                        sx={{ 
                            borderRadius: "16px",
                            py: 1.5,
                            fontWeight: 700,
                            fontSize: "1rem",
                            textTransform: "none",
                            boxShadow: "0 10px 15px -3px rgba(59, 130, 246, 0.3)",
                        }}
                    >
                        {loading ? <CircularProgress size={24} color="inherit" /> : "Apply Settings"}
                    </Button>
                </DialogActions>
            </Dialog>

            {/* Delete Confirmation Dialog */}
            <Dialog
                open={deleteConfirmOpen}
                onClose={() => !loading && setDeleteConfirmOpen(false)}
                TransitionComponent={Zoom}
                PaperProps={{
                    sx: { borderRadius: "24px", p: 1, maxWidth: "400px" }
                }}
            >
                <DialogContent sx={{ textAlign: "center", pt: 4 }}>
                    <Box sx={{ 
                        width: 60, 
                        height: 60, 
                        borderRadius: "50%", 
                        bgcolor: "#fef2f2", 
                        color: "#ef4444", 
                        display: "flex", 
                        alignItems: "center", 
                        justifyContent: "center",
                        margin: "0 auto 20px"
                    }}>
                        <WarningAmberIcon sx={{ fontSize: 35 }} />
                    </Box>
                    <Typography variant="h6" sx={{ fontWeight: 800, color: "#1e293b", mb: 1 }}>
                        Delete Device?
                    </Typography>
                    <Typography variant="body2" sx={{ color: "#64748b", px: 2 }}>
                        Are you sure you want to delete <strong>{deleteTarget?.name || deleteTarget?.deviceId}</strong>? This action cannot be undone.
                    </Typography>
                </DialogContent>
                <DialogActions sx={{ p: 3, gap: 1.5 }}>
                    <Button 
                        fullWidth
                        onClick={() => setDeleteConfirmOpen(false)}
                        disabled={loading}
                        sx={{ 
                            borderRadius: "12px", 
                            color: "#64748b", 
                            fontWeight: 600,
                            bgcolor: "#f1f5f9",
                            "&:hover": { bgcolor: "#e2e8f0" }
                        }}
                    >
                        Cancel
                    </Button>
                    <Button 
                        fullWidth
                        onClick={handleDeleteConfirm}
                        variant="contained"
                        color="error"
                        disabled={loading}
                        sx={{ 
                            borderRadius: "12px", 
                            fontWeight: 700,
                            boxShadow: "0 4px 6px -1px rgba(239, 68, 68, 0.2)"
                        }}
                    >
                        {loading ? <CircularProgress size={20} color="inherit" /> : "Delete"}
                    </Button>
                </DialogActions>
            </Dialog>
        </Box>
    );
};

export default HomePage;