import './GlobalServices.scss';
import SpeedIcon from '@mui/icons-material/Speed';
import MemoryIcon from '@mui/icons-material/Memory';
import FormatListNumberedIcon from '@mui/icons-material/FormatListNumbered';

const GlobalServices = ({ dataProvider, openLocalServiceModal }) => {

    const globalSessionService = dataProvider.globalSessionService;
    const globalDirectoryService = dataProvider.globalDirectoryService;

    return (
        <div className='grid-container'>
            <div className="grid-item">
                <h3>Global Session Service</h3>
                <div className="grid-container">
                    <div className='grid-item usage-icon'>
                        <SpeedIcon style={{ fontSize: '6em' }} />
                    </div>
                    <div className='grid-item'>
                        Global CPU usage: {parseFloat(globalSessionService["Global.CPU.Load"]).toFixed(2)}%
                    </div>
                    <div className='grid-item'>
                        GSS-Process CPU usage: {parseFloat(globalSessionService["Process.CPU.Load"]).toFixed(2)}%
                    </div>
                </div>
                <div className="grid-container" style={{ marginTop: '1.5em' }}>
                    <div className='grid-item usage-icon'>
                        <MemoryIcon style={{ fontSize: '6em' }} />
                    </div>
                    <div className='grid-item'>
                        Global Memory usage: {parseFloat(globalSessionService["Global.Memory.Load"]).toFixed(2)}%
                    </div>
                    <div className='grid-item'>
                        GSS-Process Memory usage: {parseFloat(globalSessionService["Process.Memory.Load"]).toFixed(2)}%
                    </div>
                </div>
                <ul style={{ marginTop: '1.5em' }}>
                    <h3>Local Session Services:</h3>
                    <div className='grid-container'>
                        <div className='grid-item' style={{ display: 'flex', alignItems: 'center', justifyContent: 'center' }}>
                            <FormatListNumberedIcon style={{ fontSize: '1.5em' }} />
                        </div>
                        <div className='grid-item'>
                            No. of active Services: <b>{globalSessionService["LocalSessionServices"].length}</b>
                        </div>
                        {globalSessionService["LocalSessionServices"].map((service, index) => (
                            <li key={index} className="service-item">
                                <div className='grid-item'>
                                    {service.URL}
                                </div>
                                <button className="service-details-button" style={{ marginLeft: '2em' }} onClick={() => openLocalServiceModal(service.URL)}>Show Details</button>
                            </li>
                        ))}
                    </div>
                </ul>
            </div>

            <div className="grid-item">
                <h3>Global DirectoryService</h3>
                <div className="grid-container">
                    <div className='grid-item usage-icon'>
                        <SpeedIcon style={{ fontSize: '6em' }} />
                    </div>
                    <div className='grid-item'>
                        Global CPU usage: {parseFloat(globalDirectoryService["Global.CPU.Load"]).toFixed(2)}%
                    </div>
                    <div className='grid-item'>
                        GSS-Process CPU usage: {parseFloat(globalDirectoryService["Process.CPU.Load"]).toFixed(2)}%
                    </div>
                </div>
                <div className="grid-container" style={{ marginTop: '1.5em' }}>
                    <div className='grid-item usage-icon'>
                        <MemoryIcon style={{ fontSize: '6em' }} />
                    </div>
                    <div className='grid-item'>
                        Global Memory usage: {parseFloat(globalDirectoryService["Global.Memory.Load"]).toFixed(2)}%
                    </div>
                    <div className='grid-item'>
                        GSS-Process Memory usage: {parseFloat(globalDirectoryService["Process.Memory.Load"]).toFixed(2)}%
                    </div>
                </div>
                <ul style={{ marginTop: '1.5em' }}>
                    <h3>Local Directory Services:</h3>
                    <div className='grid-container'>
                        <div className='grid-item' style={{ display: 'flex', alignItems: 'center', justifyContent: 'center' }}>
                            <FormatListNumberedIcon style={{ fontSize: '1.5em' }} />
                        </div>
                        <div className='grid-item'>
                            No. of active Services: <b>{globalDirectoryService["LocalDirectoryServices"].length}</b>
                        </div>
                        {globalDirectoryService["LocalDirectoryServices"].map((service, index) => (
                            <li key={index} className="service-item">
                                <div className='grid-item'>
                                    {service.URL}
                                </div>
                                <button className="service-details-button" style={{ marginLeft: '2em' }} onClick={() => openLocalServiceModal(service.URL)}>Show Details</button>
                            </li>
                        ))}
                    </div>
                </ul>
            </div>
        </div >
    );
};

export default GlobalServices;
