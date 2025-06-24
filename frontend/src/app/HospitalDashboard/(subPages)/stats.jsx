'use client';

import { useEffect, useState } from 'react';
import { Badge, Button, Card, Spinner, ProgressBar } from 'react-bootstrap';

const mockDistressCalls = [
  {
    id: 1,
    name: 'Raj Verma',
    age: 45,
    condition: 'Chest Pain',
    priority: 'High',
    time: '2 mins ago',
    location: 'Indiranagar',
    source: 'Distress Call',
  },
  {
    id: 2,
    name: 'Ritu Sharma',
    age: 30,
    condition: 'Severe bleeding',
    priority: 'Critical',
    time: '5 mins ago',
    location: 'Koramangala',
    source: 'Walk-In',
  },
  {
    id: 3,
    name: 'Mohd Aamir',
    age: 60,
    condition: 'Loss of consciousness',
    priority: 'Critical',
    time: 'Just now',
    location: 'BTM Layout',
    source: 'Distress Call',
  },
];

export default function EmergencyIntakeDashboard() {
  const [requests, setRequests] = useState([]);
  const [approved, setApproved] = useState([]);
  const [rejected, setRejected] = useState([]);

  // Simulate incoming distress calls
  useEffect(() => {
    const timeout = setTimeout(() => {
      setRequests(mockDistressCalls);
    }, 1000);
    return () => clearTimeout(timeout);
  }, []);

  const handleDecision = (id, decision) => {
    const patient = requests.find((r) => r.id === id);
    if (decision === 'approve') setApproved([...approved, patient]);
    if (decision === 'reject') setRejected([...rejected, patient]);
    setRequests(requests.filter((r) => r.id !== id));
  };

  return (
    <div className="container-fluid py-4">
      {/* Header */}
      <div className="d-flex justify-content-between align-items-center mb-4">
        <div>
          <h4 className="fw-bold text-danger mb-1">
            <i className="bi bi-broadcast-pin me-2"></i>Emergency Intake
          </h4>
          <p className="text-muted small mb-0">
            Real-time distress calls received by the hospital
          </p>
        </div>
        <div className="text-end">
          <div className="fw-semibold text-muted">Hospital Load</div>
          <ProgressBar now={78} label={`78%`} variant="danger" style={{ height: '8px' }} />
          <div className="small text-muted mt-1">Response Time Avg: <strong>6 min</strong></div>
        </div>
      </div>

      {/* Incoming Calls */}
      {requests.length === 0 ? (
        <div className="text-center text-muted py-5">
          <Spinner animation="border" variant="danger" />
          <p className="mt-3">Listening for new emergency calls...</p>
        </div>
      ) : (
        <div className="row g-3">
          {requests.map((req) => (
            <div className="col-md-6 col-lg-4" key={req.id}>
              <Card className="border-danger shadow-sm">
                <Card.Body>
                  <div className="d-flex justify-content-between align-items-center">
                    <h5 className="mb-1 text-danger">{req.name}</h5>
                    <Badge bg={req.priority === 'Critical' ? 'dark' : 'warning'}>
                      {req.priority}
                    </Badge>
                  </div>
                  <p className="mb-1 text-muted small">
                    <strong>{req.age}</strong> yrs • {req.condition}
                  </p>
                  <p className="mb-1 small">
                    <strong>Location:</strong> {req.location}
                  </p>
                  <p className="mb-1 small">
                    <strong>Source:</strong> {req.source}
                  </p>
                  <p className="text-muted small mb-2">⏱️ {req.time}</p>

                  <div className="d-flex justify-content-end gap-2">
                    <Button
                      variant="outline-danger"
                      size="sm"
                      onClick={() => handleDecision(req.id, 'reject')}
                    >
                      <i className="bi bi-x-circle me-1"></i> Reject
                    </Button>
                    <Button
                      variant="success"
                      size="sm"
                      onClick={() => handleDecision(req.id, 'approve')}
                    >
                      <i className="bi bi-check-circle me-1"></i> Approve
                    </Button>
                  </div>
                </Card.Body>
              </Card>
            </div>
          ))}
        </div>
      )}

      {/* Summary Bar */}
      <div className="card shadow-sm mt-5">
        <div className="card-body d-flex justify-content-between align-items-center">
          <span>
            👤 Approved: <strong>{approved.length}</strong> &nbsp;&nbsp;
            ❌ Rejected: <strong>{rejected.length}</strong> &nbsp;&nbsp;
            ⏳ Pending: <strong>{requests.length}</strong>
          </span>
          <Button variant="outline-dark" size="sm" onClick={() => location.reload()}>
            <i className="bi bi-arrow-clockwise me-1"></i> Refresh
          </Button>
        </div>
      </div>
    </div>
  );
}
